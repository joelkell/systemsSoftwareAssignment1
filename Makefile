CC = gcc
objects = serverDaemon.o backup.o daemonLoop.o givePermissions.o removeLivePermissions.o removePermissions.o startAuditWatch.o transfer.o writeAuditLogsToFile.o
headers = backup.h daemonLoop.h givePermissions.h removeLivePermissions.h removePermissions.h startAuditWatch.h transfer.h writeAuditLogsToFile.h
serverDaemonHeaders = daemonLoop.h transfer.h givePermissions.h removeLivePermissions.h removePermissions.h startAuditWatch.h
daemonLoopHeaders = backup.h transfer.h givePermissions.h removePermissions.h writeAuditLogsToFile.h

serverDaemon : $(objects)
	$(CC) -o serverDaemon $(objects) -lrt
	
serverDaemon.o : serverDaemon.c $(serverDaemonHeaders)
	$(CC) -c serverDaemon.c -lrt
	
daemonLoop.o : daemonLoop.c $(daemonLoopHeaders)
	$(CC) -c daemonLoop.c -lrt

backup.o : backup.c
	$(CC) -c backup.c -lrt

transfer.o : transfer.c
	$(CC) -c transfer.c -lrt

givePermissions.o : givePermissions.c
	$(CC) -c givePermissions.c -lrt

removePermissions.o : removePermissions.c
	$(CC) -c removePermissions.c -lrt
	
writeAuditLogsToFile.o : writeAuditLogsToFile.c
	$(CC) -c writeAuditLogsToFile.c -lrt

removeLivePermissions.o : removeLivePermissions.c
	$(CC) -c removeLivePermissions.c -lrt

startAuditWatch.o : startAuditWatch.c
	$(CC) -c startAuditWatch.c -lrt

clean: 
	rm serverDaemon $(objects)
