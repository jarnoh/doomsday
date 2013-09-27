
import sys, string




a=sys.stdin.readlines()


count=0
for i in a:
	if count==0:
		print "//", string.strip(i)
		print "{"
	if count==1 or count==2 or count==3:
		v=string.split(i)
		print v[0],",",v[1],",",v[2],","

	if count==3:
		print "},"

	count=count+1

	if len(string.strip(i))==0:
		count=0