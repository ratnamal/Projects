
# coding: utf-8

# In[ ]:


#Ratnamala Korlepara
#Hima Sujani Adike


# In[38]:


import glob
import pyspark
from pyspark.ml.feature import HashingTF, IDF, Tokenizer
from pyspark.sql import SQLContext
from pyspark import SparkConf, SparkContext
import sys
from pyspark.ml import Pipeline
from pyspark.ml.classification import DecisionTreeClassifier
from pyspark.ml.feature import StringIndexer, VectorIndexer
from pyspark.ml.evaluation import MulticlassClassificationEvaluator
from pyspark.ml.classification import LogisticRegression
from pyspark.ml.feature import RegexTokenizer, StopWordsRemover, CountVectorizer
from pyspark.ml import Pipeline
from pyspark.ml.feature import OneHotEncoder, StringIndexer, VectorAssembler
from nytimesarticle import articleAPI
from bs4 import BeautifulSoup
import requests
from nytimesarticle import articleAPI


# <center><h1>Articles Collection</h1><center>

# In[ ]:


topics=["trump","nfl","walmart","google"]
subjects=["politics","sports","business","other"]

n = articleAPI('44cccf540f9c4f51a97065dcfc6c8bd1')
index=0

def getArticlesURLs(topic):
    articles = n.search(q = topic[index], 
         fq = 'The New York Times')
    articles = articles['response']['docs']
    urls=[]
    for article in articles:
        urls.append(article['web_url'])
    return urls

for topic in topics:
    urls=getArticlesURLs(topic)
    i=0
    for url in urls:
        file=open(subjects[index]+str(i)+'.txt','w+')
        i=i+1
        page = requests.get(url)
        soup = BeautifulSoup(page.content, 'html.parser')
        for line in soup.find_all('p'):
            line=str(line)
            file.write(line)
    index=index+1


# <center><h1>Logistic Regression</h1></center>

# <h3><b>1. Creating new Spark and SQL context</b></h3>

# In[31]:


sc = SparkContext()
sqlContext = SQLContext(sc)


# <h3><b>2. Reading stopwords from a file</b></h3>

# In[32]:


words = []
stop_words=[]
file=open('stopwords.txt','r')
for line in file:
    new_line=line.replace("\n","")
    line1=new_line
    stop_words.append(line1)
stop_words[:5]


# <h3><b>3. Checking if a word is Stop word</b></h3>

# In[39]:


def isNotStopWord(word,stopWords):
    for stopword in stopWords:
        if stopword==word:
            return 0
    return 1  


# <h3>4. Retrieving topic by filename</h3>

# In[ ]:


topics=["politics","sports","business","other"]
def getArticleTopic(file):
    for topic in topics:
        if topic in file:
            return topic


# <h3><b>5. Reading all articles</b></h3>

# In[ ]:


dataList=[]
testList=[]
i=0
for file in glob.glob("/articles/*"):        
    i=i+1
    with open(file, 'rb') as f:
        data=f.read()
        words=[]
        word_tokens = data.split()
        for word in word_tokens:
            word=word.decode("utf-8", errors='ignore') 
            topic=getArticleTopic(file)
            if isNotStopWord(word,stop_words):
                words.append(word)
                words.append(" ")
        dataList.append((i,''.join(words),topic))


# <h3><b>6. Creating a Dataframe of input data and topic</b></h3>

# In[29]:


data = sqlContext.createDataFrame(dataList, ["index", "data","topic"])
data.show(5)


# <h3><b>7. Splitting data and converting into matrix of token counts</b></h3>

# In[35]:


# regular expression tokenizer
regexTokenizer = RegexTokenizer(inputCol="data", outputCol="words", pattern="\\W")
# stop words
add_stopwords = ["http","https","amp","rt","t","c","the"] 
stopwordsRemover = StopWordsRemover(inputCol="words", outputCol="filtered").setStopWords(add_stopwords)
# bag of words count
countVectors = CountVectorizer(inputCol="filtered", outputCol="features", vocabSize=10000, minDF=5)


# <b><h3>8. Creating pipeline of stages in the order of execution</h3></b>

# In[36]:


hashingTF = HashingTF(inputCol="filtered", outputCol="rawFeatures", numFeatures=10000)
idf = IDF(inputCol="rawFeatures", outputCol="features", minDocFreq=5) #minDocFreq: remove sparse terms
pipeline = Pipeline(stages=[regexTokenizer, stopwordsRemover, hashingTF, idf, label_stringIdx])


# <b><h3>9. Fitting Training Data to pipeline</h3></b>

# In[37]:


# Fit the pipeline to training documents.
pipelineFit = pipeline.fit(data)
dataset = pipelineFit.transform(data)
dataset.show(5)


# <b><h3>10. Splitting into training and test data</h3></b>

# In[ ]:


(trainingData, testData) = dataset.randomSplit([0.8, 0.2], seed = 100)


# <b><h3>11. Training model using Logistic Regression</h3></b>

# In[ ]:


lr = LogisticRegression(maxIter=20, regParam=0.3, elasticNetParam=0)
lrModel = lr.fit(trainingData)


# <b><h3>12. Testing model using test data</h3></b>

# In[21]:


predictions = lrModel.transform(testData)
predictions.filter(predictions['prediction'] == 0)     .select("data","topic","probability","label","prediction")     .orderBy("probability", ascending=False)     .show(n = 5, truncate = 30)


# <b><h3>13. Accuracy calculation for model using Logistic Regression</h3></b>

# In[47]:


evaluator = MulticlassClassificationEvaluator(predictionCol="prediction")
print('Accuracy using Logistic Regression:',evaluator.evaluate(predictions)*100)


# <center><h1>Decision Tree Classification </h1></center>

# <h3>1. Split the input data<h3>

# In[41]:


tokenizer = Tokenizer(inputCol="data", outputCol="words")
wordsData = tokenizer.transform(data)
wordsData.show(5)


# <h3><b>2. Removing Stop words and converting data into matrix of token counts</b></h3>

# In[ ]:


regexTokenizer = RegexTokenizer(inputCol="data", outputCol="words", pattern="\\W")
# stop words
add_stopwords = ["http","https","amp","rt","t","c","the"] 
stopwordsRemover = StopWordsRemover(inputCol="words", outputCol="filtered").setStopWords(add_stopwords)
# bag of words count
countVectors = CountVectorizer(inputCol="filtered", outputCol="features1", vocabSize=10000, minDF=5)


# <b><h3>3. Creating pipeline of stages in the order of execution</h3></b>

# In[ ]:


hashingTF = HashingTF(inputCol="filtered", outputCol="rawFeatures", numFeatures=10000)
idf = IDF(inputCol="rawFeatures", outputCol="features", minDocFreq=5) #minDocFreq: remove sparse terms
pipeline = Pipeline(stages=[regexTokenizer, stopwordsRemover, hashingTF, idf, label_stringIdx])


# <b><h3>4. Fitting Training Data to pipeline</h3></b>

# In[25]:


pipelineFit = pipeline.fit(data)
dataset = pipelineFit.transform(data)
dataset.show(5)


# <b><h3>5. Splitting into training and test data</h3></b>

# In[ ]:


(trainingData, testData) = dataset.randomSplit([0.8, 0.2], seed = 100)


# In[ ]:


dt = DecisionTreeClassifier()
dtModel = dt.fit(trainingData)


# <b><h3>6. Testing model using test data</h3></b>

# In[26]:


predictions1 = dtModel.transform(testData)
predictions1.filter(predictions1['prediction'] == 0)     .select("data","topic","probability","label","prediction")     .orderBy("probability", ascending=False)     .show(n = 5, truncate = 30)


# <b><h3>7. Accuracy calculation for model using Decision Tree</h3></b>

# In[46]:


evaluator = MulticlassClassificationEvaluator(predictionCol="prediction")
print('Accuracy using Decision tree:',evaluator.evaluate(predictions1)*100)

