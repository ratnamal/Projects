file=open('/home/hadoop/part-00000','r')
string=file.read()
file.close()
word_count=[]
i=0
word_dict={}
for line in string.split('\n'):
    if line=="":
        break
    i=i+1
    word_list=line.split('\t', 1)
    word=word_list[0]
    count=int(word_list[1])
    if count in word_dict:
        prev_word=word_dict[count]
        if type(prev_word)!=type([]):
             word_dict[count]=[prev_word,word]
        else:
             prev_word.append(word)
             word_dict[count]=prev_word
    else:
        word_dict[count]=word
file.close()

f=open('/home/hadoop/topWords.txt','w+')
i=0
for count,word in sorted(word_dict.items(), reverse=True):
    if i==10:
        break
    i=i+1
    if type(word_dict[count])==type([]):
        for w in word:
            cnt=str(count)
	    out=w+"\t"+cnt+"\n"
            f.write(out)
    else:
        cnt=str(count)
        out=word+"\t"+cnt+"\n"
        f.write(out) 
f.close()
f=open('/home/hadoop/cloudwords.txt','w+')
out=""
i=0
for count,word in sorted(word_dict.items(), reverse=True):
     if i==10:
        break
     i=i+1
     if type(word_dict[count])==type([]):
         for w in word:
             out=out+""+w+", "
     else:
          out=out+""+word+", "
out=out[:-2]
f.write(out) 
f.close()