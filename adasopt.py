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
from tensorflow.python.ops import control_flow_ops
from tensorflow.python.ops import state_ops
from tensorflow.python.training import moving_averages

import tensorflow as tf

class AdasOptimizer(Optimizer):
    """
    Introduction:
        For the mathematical part see https://github.com/YanaiEliyahu/AdasOptimizer,
        the `Theory` section contains the major innovation,
        and then `How ADAS works` contains more low level details that are still somewhat related to the theory.

    Compatibility:
        Tested under tensorflow versions 1.5.4 and 2.3.1.

    Arguments:
        lr: float > 0. Initial learning rate that is per feature/input (e.g. dense layer with N inputs and M outputs, will have N learning rates).
        lr2: float >= 0.  lr's Initial learning rate. (just ~1-2 per layer, additonal one because of bias)
        lr3: float >= 0. lr2's fixed learning rate. (global)
        beta_1: 0 < float < 1. Preferably close to 1. Second moments decay factor to update lr and lr2 weights.
        beta_2: 0 < float < 1. Preferably close to 1. 1/(1 - beta_2) steps back in time that `lr`s will be optimized for, larger dataset might require more nines.
        beta_3: 0 < float < 1. Preferably close to 1. Same as beta_2, but for `lr2`s.
        epsilon: float >= 0. Fuzz factor. If `None`, defaults to `K.epsilon()`.
    """
    def __init__(self,
            lr = 0.001, lr2 = .005, lr3 = .0005,
            beta_1 = 0.999, beta_2 = 0.999, beta_3 = 0.9999,
            epsilon = None, **kwargs):
        super(AdasOptimizer, self).__init__('Adas',**kwargs)
        # TODO: use self._set_hyper and self._get_hyper
        self._lr = lr
        self._lr2 = lr2
        self._lr3 = lr3
        self._beta_1 = beta_1
        self._beta_2 = beta_2
        self._beta_3 = beta_3
        if epsilon is None:
            epsilon = K.epsilon()
        self._epsilon = epsilon
        self._tf1 = tf.__version__.split(".")[0] == '1'
        if not self._tf1:
            self._varn = None
            self._is_create_slots = None
            self._curr_var = None

    def _assign(self,x,y):
        if self._tf1:
            result = K.update(x,y)
        else:
            result = state_ops.assign(x,y,use_locking=self._use_locking)
        self._updates.append(result)
        return result

    def _add(self,x,y):
        if self._tf1:
            result = K.update_add(x,y)
        else:
            result = state_ops.assign_add(x,y,use_locking=self._use_locking)
        self._updates.append(result)
        return result

    def _moving_average(self,var,value,momentum):
        if self._tf1:
            return self._assign(var,var * momentum + value * (1 - momentum))
        result = K.moving_average_update(var,value,momentum)
        self._updates.append(result)
        return result

    # TODO: fix variables' names being too convoluted in _derivatives_normalizer and _get_updates_universal_impl
    def _derivatives_normalizer(self,derivative,beta):
        if self._tf1:
            self._iterations = self._make_variable()
            self._add(self._iterations,1)
        t = K.cast(self._iterations if self._tf1 else self.iterations, K.floatx()) + 1
        lr_t = K.sqrt(1. - K.pow(self._beta_1, t))
        m = self._make_variable(0,K.int_shape(derivative),K.dtype(derivative))
        old_moments = self._make_variable(0,K.int_shape(derivative),K.dtype(derivative))
        old_moments2 = self._make_variable(0,K.int_shape(derivative),K.dtype(derivative))
        res = self._assign(old_moments2,old_moments)
        v_t = self._moving_average(m,K.square(derivative),self._beta_1)
        with tf.control_dependencies([v_t]):
            np_t = derivative * lr_t / (K.sqrt(v_t) + self._epsilon)
        with tf.control_dependencies([np_t]):
            m_t = self._moving_average(old_moments,np_t,beta)
        return (res,np_t)

    def _make_variable(self,value = 0,shape = (),dtype = K.floatx()):
        if self._tf1:
            return K.variable(K.constant(value,shape=shape, dtype=dtype))
        self._varn += 1
        name = 'unnamed_variable' + str(self._varn)
        if self._is_create_slots:
            return self.add_slot(self._curr_var,name,initializer = K.constant(value,shape=shape, dtype=dtype))
        else:
            return self.get_slot(self._curr_var,name)

    def _get_updates_universal_impl(self, grad, param):
        self._updates = []
        lr = self._make_variable(value = self._lr,shape=K.int_shape(param)[:-1], dtype=K.dtype(param))
        moment, deriv = self._derivatives_normalizer(grad,self._beta_3)
        param_t = self._add(param, - K.expand_dims(lr,len(K.int_shape(param)) - 1) * deriv)
        with tf.control_dependencies([moment]):
            lr_deriv = math_ops.reduce_sum(moment * grad,len(K.int_shape(param)) - 1)
        master_lr = self._make_variable(self._lr2)
        m2,d2 = self._derivatives_normalizer(lr_deriv,self._beta_2)
        lr_t = self._add(lr,master_lr * lr * d2)
        with tf.control_dependencies([m2]):
            master_lr_deriv2 = math_ops.reduce_sum(m2 * lr_deriv)
        m3,d3 = self._derivatives_normalizer(master_lr_deriv2,0.)
        with tf.control_dependencies([m3]):
            self._add(master_lr,self._lr3 * master_lr * d3)
        return self._updates

    def _get_updates_universal(self, param, grad = None, is_create_slots = False):
        self._curr_var = param
        self._is_create_slots = is_create_slots
        self._varn = 0
        return self._get_updates_universal_impl(grad if grad is not None else K.constant(0,shape=param.shape,dtype=K.dtype(param)),param)

    def get_updates(self, loss, params):
        return sum([self._get_updates_universal_impl(grad,var) for (grad,var) in zip(self.get_gradients(loss, params),params)],[])

    def _create_slots(self, var_list):
        for var in var_list:
            self._get_updates_universal(var,is_create_slots = True)

    def _resource_apply_dense(self, grad, var):
        return control_flow_ops.group(*self._get_updates_universal(var,grad))

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
