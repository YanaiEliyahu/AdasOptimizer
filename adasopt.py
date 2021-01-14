# MIT License
#
# Copyright (c) 2020 YanaiEliyahu
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""Implementation of Adas."""
from tensorflow.keras.optimizers import Optimizer
from tensorflow.keras import backend as K
from tensorflow.python.ops import math_ops

class AdasOptimizer(Optimizer):
    def __init__(self, lr=0.001,lr2=.005,lr3=.0002, beta_1=0.999, beta_2=0.9999, beta_3 = 0.99999,
                 epsilon=None, **kwargs):
        super(AdasOptimizer, self).__init__('Adas',**kwargs)
        self._iterations = K.variable(0, dtype='int64', name='iterations')
        self._lr = lr
        self._lr2 = lr2
        self._lr3 = lr3
        self._beta_1 = K.variable(beta_1, name='beta_1')
        self._beta_2 = K.variable(beta_2, name='beta_2')
        self._beta_3 = K.variable(beta_3, name='beta_3')
        if epsilon is None:
            epsilon = K.epsilon()
        self._epsilon = epsilon

    def _derivatives_normalizer(self,updates,derivatives,beta_1,beta_2):
        t = K.cast(self._iterations, K.floatx()) + 1
        lr_t = K.sqrt(1. - K.pow(beta_2, t))
        ms = [K.zeros(K.int_shape(p), dtype=K.dtype(p)) for p in derivatives]
        new_derivatives = [K.zeros(K.int_shape(p), dtype=K.dtype(p)) for p in derivatives]
        new_derivatives2 = []
        old_moments = [K.zeros(K.int_shape(p), dtype=K.dtype(p)) for p in derivatives]
        new_moments = []
        for p, p_t, m, om in zip(derivatives, new_derivatives, ms, old_moments):
            v_t = (beta_2 * m) + (1. - beta_2) * K.square(p)
            updates.append(K.update(m, v_t))
            np_t = p * lr_t / (K.sqrt(v_t) + self._epsilon)
            m_t = (beta_1 * om) + (1. - beta_1) * np_t
            updates.append(K.update(om, m_t))
            new_moments.append(m_t)
            new_derivatives2.append(np_t)
            updates.append(K.update(p_t,np_t))
        return (old_moments,new_derivatives2)

    def _meta_optimize(self,updates,derivatives_,beta_1,beta_2,params,default_lr,default_lr2,default_lr3):
        lrs = [K.variable(K.constant(default_lr,shape=K.int_shape(p)[:-1], dtype=K.dtype(p))) for p in params]
        moments, derivs = self._derivatives_normalizer(updates,derivatives_,beta_1,beta_2)
        for lr,deriv,moment,param,deriv2 in zip(lrs,derivs,moments,params,derivatives_):
            param_t = param - K.expand_dims(lr,len(K.int_shape(param)) - 1) * deriv
            self._updates.append(K.update(param, param_t))
            lr_deriv = math_ops.reduce_sum(moment * deriv2,len(K.int_shape(param)) - 1)
            master_lr = K.variable(default_lr2)
            m2,d2 = self._derivatives_normalizer(updates,[lr_deriv],self._beta_2,self._beta_1)
            lr_t = lr + master_lr * lr * d2[0]
            self._updates.append(K.update(lr, lr_t))
            master_lr_deriv2 = math_ops.reduce_sum(m2[0] * lr_deriv)
            m3,d3 = self._derivatives_normalizer(updates,[master_lr_deriv2],self._beta_2,self._beta_1)
            master_lr_t = master_lr + default_lr3 * master_lr * d3[0]
            self._updates.append(K.update(master_lr, master_lr_t))

    def get_updates(self, loss, params):
        grads = [a for a in self.get_gradients(loss, params)]
        self._updates = [K.update_add(self._iterations, 1)]
        self._meta_optimize(self._updates,grads,self._beta_3,self._beta_1,params,self._lr,self._lr2,self._lr3)
        return self._updates

    def get_config(self):
        config = {
            'lr': float(self._lr),
            'lr2': float(self._lr2),
            'lr3': float(self._lr3),
            'beta_1': float(K.get_value(self._beta_1)),
            'beta_2': float(K.get_value(self._beta_2)),
            'beta_3': float(K.get_value(self._beta_3)),
            'epsilon': self._epsilon
        }
        base_config = super(AdasOptimizer, self).get_config()
        return dict(list(base_config.items()) + list(config.items()))
