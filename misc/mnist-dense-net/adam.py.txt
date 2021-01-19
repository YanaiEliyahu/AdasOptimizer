#! /usr/bin/python3

from __future__ import absolute_import, division, print_function, unicode_literals

import tensorflow as tf

mnist = tf.keras.datasets.mnist

(x_train, y_train), (x_test, y_test) = mnist.load_data()
x_train, x_test = x_train / 255.0, x_test / 255.0
model = tf.keras.models.Sequential([
  tf.keras.layers.Flatten(input_shape=(28, 28)),
  tf.keras.layers.Dense(256, activation='relu'),
  tf.keras.layers.Dense(10)
])
loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)
model.compile(optimizer='adam', loss=loss_fn, metrics=['accuracy'])

model.fit(x_train, y_train, epochs=50, validation_data=(x_test, y_test))

"""
Epoch 1/50
60000/60000 [==============================] - 4s 67us/sample - loss: 0.2241 - acc: 0.9349 - val_loss: 0.1270 - val_acc: 0.9581
Epoch 2/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0920 - acc: 0.9721 - val_loss: 0.0837 - val_acc: 0.9736
Epoch 3/50
60000/60000 [==============================] - 4s 67us/sample - loss: 0.0611 - acc: 0.9814 - val_loss: 0.0726 - val_acc: 0.9768
Epoch 4/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0440 - acc: 0.9859 - val_loss: 0.0734 - val_acc: 0.9779
Epoch 5/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0313 - acc: 0.9903 - val_loss: 0.0646 - val_acc: 0.9790
Epoch 6/50
60000/60000 [==============================] - 4s 67us/sample - loss: 0.0242 - acc: 0.9923 - val_loss: 0.0713 - val_acc: 0.9796
Epoch 7/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0190 - acc: 0.9943 - val_loss: 0.0697 - val_acc: 0.9804
Epoch 8/50
60000/60000 [==============================] - 4s 66us/sample - loss: 0.0151 - acc: 0.9953 - val_loss: 0.0847 - val_acc: 0.9775
Epoch 9/50
60000/60000 [==============================] - 4s 66us/sample - loss: 0.0126 - acc: 0.9955 - val_loss: 0.0752 - val_acc: 0.9805
Epoch 10/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0115 - acc: 0.9958 - val_loss: 0.0892 - val_acc: 0.9774
Epoch 11/50
60000/60000 [==============================] - 4s 67us/sample - loss: 0.0099 - acc: 0.9970 - val_loss: 0.0831 - val_acc: 0.9795
Epoch 12/50
60000/60000 [==============================] - 4s 66us/sample - loss: 0.0084 - acc: 0.9974 - val_loss: 0.0872 - val_acc: 0.9805
Epoch 13/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0074 - acc: 0.9974 - val_loss: 0.0944 - val_acc: 0.9803
Epoch 14/50
60000/60000 [==============================] - 4s 67us/sample - loss: 0.0077 - acc: 0.9973 - val_loss: 0.0878 - val_acc: 0.9799
Epoch 15/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0060 - acc: 0.9980 - val_loss: 0.0905 - val_acc: 0.9810
Epoch 16/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0066 - acc: 0.9978 - val_loss: 0.0976 - val_acc: 0.9802
Epoch 17/50
60000/60000 [==============================] - 4s 68us/sample - loss: 0.0053 - acc: 0.9981 - val_loss: 0.0977 - val_acc: 0.9809
Epoch 18/50
60000/60000 [==============================] - 4s 66us/sample - loss: 0.0053 - acc: 0.9984 - val_loss: 0.0943 - val_acc: 0.9810
Epoch 19/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0055 - acc: 0.9981 - val_loss: 0.1110 - val_acc: 0.9785
Epoch 20/50
60000/60000 [==============================] - 4s 67us/sample - loss: 0.0049 - acc: 0.9985 - val_loss: 0.0929 - val_acc: 0.9825
Epoch 21/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0038 - acc: 0.9987 - val_loss: 0.0913 - val_acc: 0.9818
Epoch 22/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0058 - acc: 0.9982 - val_loss: 0.1132 - val_acc: 0.9788
Epoch 23/50
60000/60000 [==============================] - 4s 67us/sample - loss: 0.0039 - acc: 0.9986 - val_loss: 0.1066 - val_acc: 0.9826
Epoch 24/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0042 - acc: 0.9986 - val_loss: 0.1108 - val_acc: 0.9813
Epoch 25/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0044 - acc: 0.9985 - val_loss: 0.1235 - val_acc: 0.9808
Epoch 26/50
60000/60000 [==============================] - 4s 68us/sample - loss: 0.0036 - acc: 0.9990 - val_loss: 0.1167 - val_acc: 0.9823
Epoch 27/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0040 - acc: 0.9987 - val_loss: 0.1087 - val_acc: 0.9815
Epoch 28/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0033 - acc: 0.9990 - val_loss: 0.1123 - val_acc: 0.9821
Epoch 29/50
60000/60000 [==============================] - 4s 68us/sample - loss: 0.0055 - acc: 0.9982 - val_loss: 0.1825 - val_acc: 0.9727
Epoch 30/50
60000/60000 [==============================] - 4s 66us/sample - loss: 0.0046 - acc: 0.9985 - val_loss: 0.1266 - val_acc: 0.9820
Epoch 31/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0038 - acc: 0.9988 - val_loss: 0.1166 - val_acc: 0.9823
Epoch 32/50
60000/60000 [==============================] - 4s 67us/sample - loss: 0.0040 - acc: 0.9985 - val_loss: 0.1306 - val_acc: 0.9805
Epoch 33/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0026 - acc: 0.9991 - val_loss: 0.1503 - val_acc: 0.9792
Epoch 34/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0045 - acc: 0.9984 - val_loss: 0.1283 - val_acc: 0.9821
Epoch 35/50
60000/60000 [==============================] - 4s 67us/sample - loss: 0.0019 - acc: 0.9995 - val_loss: 0.1291 - val_acc: 0.9825
Epoch 36/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0048 - acc: 0.9983 - val_loss: 0.1835 - val_acc: 0.9762
Epoch 37/50
60000/60000 [==============================] - 4s 68us/sample - loss: 0.0023 - acc: 0.9994 - val_loss: 0.1374 - val_acc: 0.9811
Epoch 38/50
60000/60000 [==============================] - 4s 68us/sample - loss: 0.0040 - acc: 0.9986 - val_loss: 0.1461 - val_acc: 0.9809
Epoch 39/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0033 - acc: 0.9990 - val_loss: 0.1306 - val_acc: 0.9836
Epoch 40/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0038 - acc: 0.9988 - val_loss: 0.1538 - val_acc: 0.9807
Epoch 41/50
60000/60000 [==============================] - 4s 68us/sample - loss: 0.0023 - acc: 0.9993 - val_loss: 0.1404 - val_acc: 0.9818
Epoch 42/50
60000/60000 [==============================] - 4s 66us/sample - loss: 0.0046 - acc: 0.9986 - val_loss: 0.1379 - val_acc: 0.9816
Epoch 43/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0021 - acc: 0.9994 - val_loss: 0.1515 - val_acc: 0.9798
Epoch 44/50
60000/60000 [==============================] - 4s 68us/sample - loss: 0.0029 - acc: 0.9991 - val_loss: 0.1487 - val_acc: 0.9810
Epoch 45/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0036 - acc: 0.9990 - val_loss: 0.1725 - val_acc: 0.9789
Epoch 46/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0037 - acc: 0.9990 - val_loss: 0.1612 - val_acc: 0.9812
Epoch 47/50
60000/60000 [==============================] - 4s 68us/sample - loss: 0.0027 - acc: 0.9992 - val_loss: 0.1528 - val_acc: 0.9812
Epoch 48/50
60000/60000 [==============================] - 4s 66us/sample - loss: 0.0031 - acc: 0.9990 - val_loss: 0.1461 - val_acc: 0.9827
Epoch 49/50
60000/60000 [==============================] - 4s 65us/sample - loss: 0.0013 - acc: 0.9996 - val_loss: 0.1580 - val_acc: 0.9815
Epoch 50/50
60000/60000 [==============================] - 4s 67us/sample - loss: 0.0037 - acc: 0.9990 - val_loss: 0.1565 - val_acc: 0.9829
"""
