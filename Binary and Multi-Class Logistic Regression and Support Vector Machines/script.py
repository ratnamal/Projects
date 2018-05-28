import numpy as np
from scipy.io import loadmat
from scipy.optimize import minimize
from sklearn import svm

def preprocess():
    """ 
     Input:
     Although this function doesn't have any input, you are required to load
     the MNIST data set from file 'mnist_all.mat'.

     Output:
     train_data: matrix of training set. Each row of train_data contains 
       feature vector of a image
     train_label: vector of label corresponding to each image in the training
       set
     validation_data: matrix of training set. Each row of validation_data 
       contains feature vector of a image
     validation_label: vector of label corresponding to each image in the 
       training set
     test_data: matrix of training set. Each row of test_data contains 
       feature vector of a image
     test_label: vector of label corresponding to each image in the testing
       set
    """

    mat = loadmat('mnist_all.mat')  # loads the MAT object as a Dictionary

    n_feature = mat.get("train1").shape[1]
    n_sample = 0
    for i in range(10):
        n_sample = n_sample + mat.get("train" + str(i)).shape[0]
    n_validation = 1000
    n_train = n_sample - 10 * n_validation

    # Construct validation data
    validation_data = np.zeros((10 * n_validation, n_feature))
    for i in range(10):
        validation_data[i * n_validation:(i + 1) * n_validation, :] = mat.get("train" + str(i))[0:n_validation, :]

    # Construct validation label
    validation_label = np.ones((10 * n_validation, 1))
    for i in range(10):
        validation_label[i * n_validation:(i + 1) * n_validation, :] = i * np.ones((n_validation, 1))

    # Construct training data and label
    train_data = np.zeros((n_train, n_feature))
    train_label = np.zeros((n_train, 1))
    temp = 0
    for i in range(10):
        size_i = mat.get("train" + str(i)).shape[0]
        train_data[temp:temp + size_i - n_validation, :] = mat.get("train" + str(i))[n_validation:size_i, :]
        train_label[temp:temp + size_i - n_validation, :] = i * np.ones((size_i - n_validation, 1))
        temp = temp + size_i - n_validation

    # Construct test data and label
    n_test = 0
    for i in range(10):
        n_test = n_test + mat.get("test" + str(i)).shape[0]
    test_data = np.zeros((n_test, n_feature))
    test_label = np.zeros((n_test, 1))
    temp = 0
    for i in range(10):
        size_i = mat.get("test" + str(i)).shape[0]
        test_data[temp:temp + size_i, :] = mat.get("test" + str(i))
        test_label[temp:temp + size_i, :] = i * np.ones((size_i, 1))
        temp = temp + size_i

    # Delete features which don't provide any useful information for classifiers
    sigma = np.std(train_data, axis=0)
    index = np.array([])
    for i in range(n_feature):
        if (sigma[i] > 0.001):
            index = np.append(index, [i])
    train_data = train_data[:, index.astype(int)]
    validation_data = validation_data[:, index.astype(int)]
    test_data = test_data[:, index.astype(int)]

    # Scale data to 0 and 1
    train_data /= 255.0
    validation_data /= 255.0
    test_data /= 255.0

    return train_data, train_label, validation_data, validation_label, test_data, test_label


def sigmoid(z):
    return 1.0 / (1.0 + np.exp(-z))


def blrObjFunction(initialWeights, *args):
    """
    blrObjFunction computes 2-class Logistic Regression error function and
    its gradient.

    Input:
        initialWeights: the weight vector (w_k) of size (D + 1) x 1 
        train_data: the data matrix of size N x D
        labeli: the label vector (y_k) of size N x 1 where each entry can be either 0 or 1 representing the label of corresponding feature vector

    Output: 
        error: the scalar value of error function of 2-class logistic regression
        error_grad: the vector of size (D+1) x 1 representing the gradient of
                    error function
    """
    train_data, labeli = args

    n_data = train_data.shape[0]
    n_features = train_data.shape[1]
    error = 0
    error_grad = np.zeros((n_features + 1, 1))

    ##################
    # YOUR CODE HERE #
    ##################
    # HINT: Do not forget to add the bias term to your input data

    t = np.hstack((train_data, np.ones((train_data.shape[0], 1)))) 
    sigma = np.dot(t,initialWeights) 
    z = sigmoid(sigma)
    z = z[:, np.newaxis]

    a1 = labeli * np.log(z)
    a2 = (1-labeli) * np.log(1-z)
    a3 = np.sum(a1 + a2)
    a4 = a3 / n_data
    error = (-1) * a4
    
    b1 = (z - labeli) 
    b2 = np.dot(t.T,b1)  
    error_grad = b2 / n_data
    error_grad = np.squeeze(np.array(error_grad))
    
    return error, error_grad


def blrPredict(W, data):
    """
     blrObjFunction predicts the label of data given the data and parameter W 
     of Logistic Regression
     
     Input:
         W: the matrix of weight of size (D + 1) x 10. Each column is the weight 
         vector of a Logistic Regression classifier.
         X: the data matrix of size N x D
         
     Output: 
         label: vector of size N x 1 representing the predicted label of 
         corresponding feature vector given in data matrix

    """
    label = np.zeros((data.shape[0], 1))

    ##################
    # YOUR CODE HERE #
    ##################
    # HINT: Do not forget to add the bias term to your input data

    t = np.hstack((data, np.ones((data.shape[0], 1))))
    sigma = np.dot(t,W)
    z = sigmoid(sigma)

    label = np.argmax(z,1)
    label = label[:, np.newaxis]
    
    return label


def mlrObjFunction(params, *args):
    """
    mlrObjFunction computes multi-class Logistic Regression error function and
    its gradient.

    Input:
        initialWeights: the weight vector of size (D + 1) x 1
        train_data: the data matrix of size N x D
        labeli: the label vector of size N x 1 where each entry can be either 0 or 1
                representing the label of corresponding feature vector

    Output:
        error: the scalar value of error function of multi-class logistic regression
        error_grad: the vector of size (D+1) x 10 representing the gradient of
                    error function
    """
    
    train_data, labeli = args
    
    n_data = train_data.shape[0]
    n_feature = train_data.shape[1]
    error = 0
    error_grad = np.zeros((n_feature + 1, n_class))

    ##################
    # YOUR CODE HERE #
    ##################
    # HINT: Do not forget to add the bias term to your input data

    t = np.hstack((train_data, np.ones((train_data.shape[0], 1))))
    initialWeights_b=params.reshape(716,10)
    sig = np.dot(t,initialWeights_b) 
    sigma1 = np.exp(sig)
    sigma2 = 0
    
    for j in range(0,10):
        sigma2 = sigma2 + np.exp(np.dot(t,initialWeights_b[:,j]))
        
    sigma2 = sigma2[:, np.newaxis]
    sigma = sigma1/sigma2
    c1 = labeli * np.log(sigma)    
    c2 = np.sum(c1)
    error = ((-1) * c2) /n_data
    
    d1 = sigma - labeli 
    error_grad = np.dot(t.T,d1)/ n_data 
    error_grad = error_grad.flatten()
    
    return error, error_grad


def mlrPredict(W, data):
    """
     mlrObjFunction predicts the label of data given the data and parameter W
     of Logistic Regression

     Input:
         W: the matrix of weight of size (D + 1) x 10. Each column is the weight
         vector of a Logistic Regression classifier.
         X: the data matrix of size N x D

     Output:
         label: vector of size N x 1 representing the predicted label of
         corresponding feature vector given in data matrix

    """
    label = np.zeros((data.shape[0], 1))

    ##################
    # YOUR CODE HERE #
    ##################
    # HINT: Do not forget to add the bias term to your input data

    t = np.hstack((data, np.ones((data.shape[0], 1)))) #N x D+1   
    sigma1 = np.exp(np.dot(t,W)) #N x 10
    sigma2 = 0
    for j in range(0,10):
        sigma2 = sigma2 + np.exp(np.dot(t,W[:,j]))
    sigma2 = sigma2[:, np.newaxis]
    
    sigma = sigma1/sigma2
    
    label = np.argmax(sigma,1)
    label = label[:, np.newaxis]
    
    return label


"""
Script for Logistic Regression
"""
train_data, train_label, validation_data, validation_label, test_data, test_label = preprocess()

# number of classes
n_class = 10

# number of training samples
n_train = train_data.shape[0]

# number of features
n_feature = train_data.shape[1]

Y = np.zeros((n_train, n_class))
for i in range(n_class):
    Y[:, i] = (train_label == i).astype(int).ravel()

# Logistic Regression with Gradient Descent
W = np.zeros((n_feature + 1, n_class))
initialWeights = np.zeros((n_feature + 1, 1))
opts = {'maxiter': 100}
for i in range(n_class):
    labeli = Y[:, i].reshape(n_train, 1)
    args = (train_data, labeli)
    nn_params = minimize(blrObjFunction, initialWeights, jac=True, args=args, method='CG', options=opts)
    W[:, i] = nn_params.x.reshape((n_feature + 1,))

# Find the accuracy on Training Dataset
predicted_label = blrPredict(W, train_data)
print('\n Training set Accuracy:' + str(100 * np.mean((predicted_label == train_label.reshape(train_label.shape[0],1)).astype(float))) + '%')

# Find the accuracy on Validation Dataset
predicted_label = blrPredict(W, validation_data)
print('\n Validation set Accuracy:' + str(100 * np.mean((predicted_label == validation_label.reshape(validation_label.shape[0],1)).astype(float))) + '%')

# Find the accuracy on Testing Dataset
predicted_label = blrPredict(W, test_data)
print('\n Testing set Accuracy:' + str(100 * np.mean((predicted_label == test_label.reshape(test_label.shape[0],1)).astype(float))) + '%')

"""
Script for Support Vector Machine
"""

print('\n\n--------------SVM-------------------\n\n')
##################
# YOUR CODE HERE #
##################

#Linear Kernel
svc = svm.SVC(kernel='linear')
svc.fit(train_data, train_label.ravel())

train_pred = svc.predict(train_data)
train_acc = svc.score(train_data, train_label)
print("Training set accuracy: " +str(train_acc))

valid_pred = svc.predict(validation_data)
valid_acc = svc.score(validation_data, validation_label)
print("Validation set accuracy: " +str(valid_acc))

test_pred = svc.predict(test_data)
test_acc = svc.score(test_data, test_label)
print("Testing set accuracy: " +str(test_acc))

#Gamma = 1.0
svc = svm.SVC(gamma=1.0)
svc.fit(train_data, train_label.ravel())

train_pred = svc.predict(train_data)
train_acc = svc.score(train_data, train_label)
print("Training accuracy: " +str(train_acc))

valid_pred = svc.predict(validation_data)
valid_acc = svc.score(validation_data, validation_label)
print("Validation accuracy: " +str(valid_acc))

test_pred = svc.predict(test_data)
test_acc = svc.score(test_data, test_label)
print("Test accuracy: " +str(test_acc))

#Gamma = default
svc = svm.SVC()
svc.fit(train_data, train_label.ravel())

train_pred = svc.predict(train_data)
train_acc = svc.score(train_data, train_label)
print("Training accuracy: " +str(train_acc))

valid_pred = svc.predict(validation_data)
valid_acc = svc.score(validation_data, validation_label)
print("Validation accuracy: " +str(valid_acc))

test_pred = svc.predict(test_data)
test_acc = svc.score(test_data, test_label)
print("Test accuracy: " +str(test_acc))

#Gamma = default and varying value of C
Cs = [1,10,20,30,40,50,60,70,80,90,100]
for C in Cs:
	svc = svm.SVC(C=C)
	svc.fit(train_data, train_label.ravel())
	train_pred = svc.predict(train_data)
	train_acc = svc.score(train_data, train_label)
	print("train accuracy with C = " + str(C) + ": "+str(train_acc))

	valid_pred = svc.predict(validation_data)
	valid_acc = svc.score(validation_data, validation_label)
	print("validation accuracy with C = " + str(C) + ": "+str(valid_acc))

	test_pred = svc.predict(test_data)
	test_acc = svc.score(test_data, test_label)
	print("test accuracy with C = " + str(C) + ": "+ str(test_acc))

"""
Script for Extra Credit Part
"""
# FOR EXTRA CREDIT ONLY
W_b = np.zeros((n_feature + 1, n_class))
initialWeights_b = np.zeros((n_feature + 1, n_class))
opts_b = {'maxiter': 100}

args_b = (train_data, Y)
nn_params = minimize(mlrObjFunction, initialWeights_b, jac=True, args=args_b, method='CG', options=opts_b)
W_b = nn_params.x.reshape((n_feature + 1, n_class))

# Find the accuracy on Training Dataset
predicted_label_b = mlrPredict(W_b, train_data)
print('\n Training set Accuracy:' + str(100 * np.mean((predicted_label_b == train_label.reshape(train_label.shape[0],1)).astype(float))) + '%')

# Find the accuracy on Validation Dataset
predicted_label_b = mlrPredict(W_b, validation_data)
print('\n Validation set Accuracy:' + str(100 * np.mean((predicted_label_b == validation_label.reshape(validation_label.shape[0],1)).astype(float))) + '%')

# Find the accuracy on Testing Dataset
predicted_label_b = mlrPredict(W_b, test_data)
print('\n Testing set Accuracy:' + str(100 * np.mean((predicted_label_b == test_label.reshape(test_label.shape[0],1)).astype(float))) + '%')
