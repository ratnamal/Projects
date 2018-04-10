
# coding: utf-8

# In[5]:


import numpy as np
from scipy.optimize import minimize
from scipy.io import loadmat
from numpy.linalg import det, inv
from math import sqrt, pi
import scipy.io
import matplotlib.pyplot as plt
import pickle
import sys
from __future__ import division

#LDA
def ldaLearn(X,y):
    # Inputs
    # X - a N x d matrix with each row corresponding to a training example
    # y - a N x 1 column vector indicating the labels for each training example
    #
    # Outputs
    # means - A d x k matrix containing learnt means for each of the k classes
    # covmat - A single d x d learnt covariance matrix 
    
    # IMPLEMENT THIS METHOD 
    
    means = np.ndarray((5,2))
    for c1 in range(1,6):
        sum_v = np.zeros(2)
        cnt =0
        for i in range(0, len(X)):
            if y[i] == c1:
                sum_v = sum_v + X[i]
                cnt = cnt+1
        means[c1-1] = sum_v/cnt

    covmat = np.cov(X.T)
    return means,covmat

#QDA
def qdaLearn(X,y):
    # Inputs
    # X - a N x d matrix with each row corresponding to a training example
    # y - a N x 1 column vector indicating the labels for each training example
    #
    # Outputs
    # means - A d x k matrix containing learnt means for each of the k classes
    # covmats - A list of k d x d learnt covariance matrices for each of the k classes
    
    # IMPLEMENT THIS METHOD
    
    means = np.ndarray((5,2))
    for c1 in range(1,6):
        sum_v = np.zeros(2)
        cnt =0
        for i in range(0, len(X)):
            if y[i] == c1:
                sum_v = sum_v + X[i]
                cnt = cnt+1
        means[c1-1] = sum_v/cnt
    
    X1, X2, X3, X4, X5 = [[] for Xx in range(5)]
    for c in range(0,len(X)):
        if y[c]==1:
            X1.append(X[c,].tolist())
        elif y[c]==2:
            X2.append(X[c,].tolist())
        elif y[c]==3:
            X3.append(X[c,].tolist())
        elif y[c]==4:
            X4.append(X[c,].tolist())
        else:
            X5.append(X[c,].tolist())

    covmats = []
    covmats.append(np.cov(np.asarray(X1).T))
    covmats.append(np.cov(np.asarray(X2).T))
    covmats.append(np.cov(np.asarray(X3).T))
    covmats.append(np.cov(np.asarray(X4).T))
    covmats.append(np.cov(np.asarray(X5).T))
    return means,covmats

#Testing LDA
def ldaTest(means,covmat,Xtest,ytest):
    # Inputs
    # means, covmat - parameters of the LDA model
    # Xtest - a N x d matrix with each row corresponding to a test example
    # ytest - a N x 1 column vector indicating the labels for each test example
    # Outputs
    # acc - A scalar accuracy value
    # ypred - N x 1 column vector indicating the predicted labels

    # IMPLEMENT THIS METHOD
    
    t1 = 1/((2*pi)**(2/2)*(sqrt(np.linalg.det(covmat))))
    ypred = np.ndarray((len(Xtest),1))
    for x in range(0,len(Xtest)):
        yprob = []
        for c in range(1,6):
            t2 = np.dot((-1/2),np.transpose(np.subtract(Xtest[x,],means[c-1,])))
            t3 = np.dot(t2.T,np.linalg.inv(covmat))
            t4 = np.dot(t3,np.subtract(Xtest[x,],means[c-1,]).T)
            t5 = np.exp(t4)
            t6 = np.dot(t1,t5)
            yprob.append(t6) 
        ypred[x][0] = yprob.index(max(yprob))+1
    
    cnt = 0
    for yy in range(len(ytest)):
        if ypred[yy,] == ytest[yy,]:
            cnt +=1
    
    acc = cnt/len(ytest)
    return acc,ypred

#Testing QDA
def qdaTest(means,covmats,Xtest,ytest):
    # Inputs
    # means, covmats - parameters of the QDA model
    # Xtest - a N x d matrix with each row corresponding to a test example
    # ytest - a N x 1 column vector indicating the labels for each test example
    # Outputs
    # acc - A scalar accuracy value
    # ypred - N x 1 column vector indicating the predicted labels

    # IMPLEMENT THIS METHOD
    
    ypred = np.ndarray((len(Xtest),1))
    for x in range(0,len(Xtest)):
        yprob = []
        for c in range(0,5):
            t1 = 1/((2*pi)**(2/2)*(sqrt(np.linalg.det(covmats[c]))))
            t2 = np.dot((-1/2),np.transpose(np.subtract(Xtest[x,],means[c,])))
            t3 = np.dot(t2.T,np.linalg.inv(covmats[c]))
            t4 = np.dot(t3,np.subtract(Xtest[x,],means[c,]).T)
            t5 = np.exp(t4)
            t6 = np.dot(t1,t5)
            yprob.append(t6)
        ypred[x][0] = yprob.index(max(yprob))+1
    
    cnt = 0
    for yy in range(len(ytest)):
        if ypred[yy,] == ytest[yy,]:
            cnt +=1
    
    acc = cnt/len(ytest)
    return acc,ypred

#Linear regression
def learnOLERegression(X,y):
    # Inputs:                                                         
    # X = N x d 
    # y = N x 1                                                               
    # Output: 
    # w = d x 1 
	
    # IMPLEMENT THIS METHOD 
    
    w = np.dot(np.dot(np.linalg.inv(np.dot(X.T,X)),X.T),y) 
    return w

#Ridge regression
def learnRidgeRegression(X,y,lambd):
    # Inputs:
    # X = N x d                                                               
    # y = N x 1 
    # lambd = ridge parameter (scalar)
    # Output:                                                                  
    # w = d x 1                                                                

    # IMPLEMENT THIS METHOD 
    
    I = np.identity(X.shape[1])
    res = np.linalg.inv(np.add(lambd * I,np.dot(X.T,X)))
    w = np.dot(np.dot(res,X.T),y)
    return w

#To calculate Mean Squared Error
def testOLERegression(w,Xtest,ytest):
    # Inputs:
    # w = d x 1
    # Xtest = N x d
    # ytest = X x 1
    # Output:
    # mse
    
    # IMPLEMENT THIS METHOD
    
    mse = (np.sum(np.subtract(ytest,np.dot(Xtest,w))**2))/Xtest.shape[0]
    return mse

#Using Gradient Descent for Ridge Regression Learning
def regressionObjVal(w, X, y, lambd):

    # compute squared error (scalar) and gradient of squared error with respect
    # to w (vector) for the given data X and y and the regularization parameter
    # lambda                                                                  

    # IMPLEMENT THIS METHOD  
    
    w = np.asmatrix(w)
    w = np.transpose(w)
    
    enter = np.subtract(y,np.dot(X,w)) 
    error_part2 = (lambd * (np.dot(w.T,w)))/2
    error_part1 = (np.dot(np.transpose(enter), enter))/2
    error = error_part1 + error_part2

    error_grad_part1 = np.dot(np.transpose(X),enter)
    error_grad_part2 = lambd * w

    error_grad = np.subtract(error_grad_part2,error_grad_part1)
    error_grad = np.squeeze(np.array(error_grad))
    return error, error_grad

#Non-Linear Regression
def mapNonLinear(x,p):
    # Inputs:                                                                  
    # x - a single column vector (N x 1)                                       
    # p - integer (>= 0)                                                       
    # Outputs:                                                                 
    # Xp - (N x (p+1)) 
	
    # IMPLEMENT THIS METHOD
    
    Xp = np.zeros(shape=(len(x),p+1))

    for j in range (p+1):
        for i in range(len(x)):
            Xp[i][j] = x[i]**j
    return Xp

# Main script

# Problem 1
# load the sample data                                                                 
if sys.version_info.major == 2:
    X,y,Xtest,ytest = pickle.load(open('sample.pickle','rb'))
else:
    X,y,Xtest,ytest = pickle.load(open('sample.pickle','rb'),encoding = 'latin1')

# LDA
means,covmat = ldaLearn(X,y)
ldaacc,ldares = ldaTest(means,covmat,Xtest,ytest)
print('LDA Accuracy = '+str(ldaacc))
# QDA
means,covmats = qdaLearn(X,y)
qdaacc,qdares = qdaTest(means,covmats,Xtest,ytest)
print('QDA Accuracy = '+str(qdaacc))

# plotting boundaries
x1 = np.linspace(-5,20,100)
x2 = np.linspace(-5,20,100)
xx1,xx2 = np.meshgrid(x1,x2)
xx = np.zeros((x1.shape[0]*x2.shape[0],2))
xx[:,0] = xx1.ravel()
xx[:,1] = xx2.ravel()

fig = plt.figure(figsize=[12,6])
plt.subplot(1, 2, 1)

zacc,zldares = ldaTest(means,covmat,xx,np.zeros((xx.shape[0],1)))
plt.contourf(x1,x2,zldares.reshape((x1.shape[0],x2.shape[0])),alpha=0.3)
plt.scatter(Xtest[:,0],Xtest[:,1],c=ytest)
plt.title('LDA')

plt.subplot(1, 2, 2)

zacc,zqdares = qdaTest(means,covmats,xx,np.zeros((xx.shape[0],1)))
plt.contourf(x1,x2,zqdares.reshape((x1.shape[0],x2.shape[0])),alpha=0.3)
plt.scatter(Xtest[:,0],Xtest[:,1],c=ytest)
plt.title('QDA')

plt.show()
# Problem 2
if sys.version_info.major == 2:
    X,y,Xtest,ytest = pickle.load(open('diabetes.pickle','rb'))
else:
    X,y,Xtest,ytest = pickle.load(open('diabetes.pickle','rb'),encoding = 'latin1')

# add intercept
X_i = np.concatenate((np.ones((X.shape[0],1)), X), axis=1)
Xtest_i = np.concatenate((np.ones((Xtest.shape[0],1)), Xtest), axis=1)

w = learnOLERegression(X,y)
mle = testOLERegression(w,Xtest,ytest)

w_i = learnOLERegression(X_i,y)
mle_i = testOLERegression(w_i,Xtest_i,ytest)

print('MSE without intercept '+str(mle))
print('MSE with intercept '+str(mle_i))

# Problem 3
k = 101
lambdas = np.linspace(0, 1, num=k)
i = 0
mses3_train = np.zeros((k,1))
mses3 = np.zeros((k,1))
for lambd in lambdas:
    w_l = learnRidgeRegression(X_i,y,lambd)
    mses3_train[i] = testOLERegression(w_l,X_i,y)
    mses3[i] = testOLERegression(w_l,Xtest_i,ytest)
    i = i + 1
fig = plt.figure(figsize=[12,6])
plt.subplot(1, 2, 1)
plt.plot(lambdas,mses3_train)
plt.title('MSE for Train Data')
plt.subplot(1, 2, 2)
plt.plot(lambdas,mses3)
plt.title('MSE for Test Data')

plt.show()
# Problem 4
k = 101
lambdas = np.linspace(0, 1, num=k)
i = 0
mses4_train = np.zeros((k,1))
mses4 = np.zeros((k,1))
opts = {'maxiter' : 20}    # Preferred value.                                                
w_init = np.ones((X_i.shape[1],1))
for lambd in lambdas:
    args = (X_i, y, lambd)
    w_l = minimize(regressionObjVal, w_init, jac=True, args=args,method='CG', options=opts)
    w_l = np.transpose(np.array(w_l.x))
    w_l = np.reshape(w_l,[len(w_l),1])
    mses4_train[i] = testOLERegression(w_l,X_i,y)
    mses4[i] = testOLERegression(w_l,Xtest_i,ytest)
    i = i + 1
fig = plt.figure(figsize=[12,6])
plt.subplot(1, 2, 1)
plt.plot(lambdas,mses4_train)
plt.plot(lambdas,mses3_train)
plt.title('MSE for Train Data')
plt.legend(['Using scipy.minimize','Direct minimization'])

plt.subplot(1, 2, 2)
plt.plot(lambdas,mses4)
plt.plot(lambdas,mses3)
plt.title('MSE for Test Data')
plt.legend(['Using scipy.minimize','Direct minimization'])
plt.show()


# Problem 5
pmax = 7
lambda_opt = 0.06 # REPLACE THIS WITH lambda_opt estimated from Problem 3
mses5_train = np.zeros((pmax,2))
mses5 = np.zeros((pmax,2))
for p in range(pmax):
    Xd = mapNonLinear(X[:,2],p)
    Xdtest = mapNonLinear(Xtest[:,2],p)
    w_d1 = learnRidgeRegression(Xd,y,0)
    mses5_train[p,0] = testOLERegression(w_d1,Xd,y)
    mses5[p,0] = testOLERegression(w_d1,Xdtest,ytest)
    w_d2 = learnRidgeRegression(Xd,y,lambda_opt)
    mses5_train[p,1] = testOLERegression(w_d2,Xd,y)
    mses5[p,1] = testOLERegression(w_d2,Xdtest,ytest)

fig = plt.figure(figsize=[12,6])
plt.subplot(1, 2, 1)
plt.plot(range(pmax),mses5_train)
plt.title('MSE for Train Data')
plt.legend(('No Regularization','Regularization'))
plt.subplot(1, 2, 2)
plt.plot(range(pmax),mses5)
plt.title('MSE for Test Data')
plt.legend(('No Regularization','Regularization'))
plt.show()

