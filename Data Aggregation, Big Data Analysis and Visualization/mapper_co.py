#!/usr/bin/python
import sys

for data in sys.stdin:
    file=open('/home/hadoop/stopwords.txt','r')
    topwords_file=open('/home/hadoop/topWords.txt','r')

    stop_words=[]
    word_list=[]
    topwords_list=[]

    for line in file:
        new_line=line.replace("\n","")
        stop_words.append(new_line)

    word_tokens = data.split()
        #stemmer = PorterStemmer()
        #filtered_data = [stemmer.stem(word) for word in word_tokens]
    words = [word for word in word_tokens if not word in stop_words]

    for line in topwords_file:
        word=line.split('\t',1)
        word=word[0]
        topwords_list.append(word)

    for word in topwords_list:
        for neighbour in words:
            if word==neighbour:
                continue
            elif neighbour in topwords_list:
	        continue
            else:
                revStr=neighbour.lower()+"-"+word.lower()
		wordpair=word.lower()+"-"+neighbour.lower()
                if revStr in word_list:
                    continue                
                word_list.append(wordpair)
                print '%s\t%s' % (wordpair, 1)