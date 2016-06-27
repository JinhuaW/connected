#!/usr/bin/python
import time  
import threading
import os

def run(testid, testnum):
	cnt = 0
	err_cnt = 0
	command = "../web_server/exchange"
	while cnt<testnum:
		command += ' ' +  testid + ' echo'
		command += ' \"' + time.asctime(time.localtime(time.time())) + '\" >/dev/null'
		n = os.system(command)
		if (n != 0):
			err_cnt += 1	
		cnt += 1
	print testid,'total missed',err_cnt

def main(clientnum, testnum):
	threads = []
	nloops = range(0, clientnum)
	for num in nloops:
		testid = 'test_' + str(num)
		t = threading.Thread(target=run, args=(testid, testnum))
		threads.append(t)
	for num in nloops:
		threads[num].start()
	for num in nloops:
		threads[num].join()

if __name__=='__main__':
	main(3, 1000)  
