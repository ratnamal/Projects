#!/usr/bin/python

## Mapper
# Splits data into list of words and removes stop words
#
## Output:
# word1 1
# word2 1


import sys
for data in sys.stdin:
    file=open('/home/hadoop/stopwords.txt','r')
    stop_words=[]
    for line in file:
        new_line=line.replace("\n","")
        stop_words.append(new_line)
    word_tokens = data.split()
        #stemmer = PorterStemmer()
        #filtered_data = [stemmer.stem(word) for word in word_tokens]
    words = [word for word in word_tokens if not word in stop_words]
    for word in words:
        print '%s\t%s' % (word, 1)
