lib_LIBRARIES = libSantiago.a
						
# where to install the headers on the system
Santiago_adir = $(includedir)/Santiago
AppServerdir = $(includedir)/Santiago/AppServer
Fastcgidir = $(includedir)/Santiago/Fastcgi
fastcgi_devkitdir = $(includedir)/Santiago/fastcgi_devkit
SantiagoDBTablesdir = $(includedir)/Santiago/SantiagoDBTables
ThreadSpecificVardir = $(includedir)/Santiago/ThreadSpecificVar
Userdir = $(includedir)/Santiago/User
SingleNodedir = $(includedir)/Santiago/User/SingleNode
Utilsdir = $(includedir)/Santiago/Utils

# the list of header files that belong to the library (to be installed later)
Santiago_a_HEADERS =	\
						src/Santiago/ErrorCategory.h  \
						src/Santiago/HTTPDefinitions.h  

AppServer_HEADERS = \
		       				src/Santiago/AppServer/Request.h \
		      				src/Santiago/AppServer/RequestHandlerBase.h \
		     			        src/Santiago/AppServer/ServerBase.h 
Fastcgi_HEADERS = \
						src/Santiago/Fastcgi/Acceptor.h \
						src/Santiago/Fastcgi/Common.h \
						src/Santiago/Fastcgi/Connection.h \
						src/Santiago/Fastcgi/ConnectionData.h \
						src/Santiago/Fastcgi/PacketSocket.h \
						src/Santiago/Fastcgi/PairParseFunctions.h \
						src/Santiago/Fastcgi/RecordSocket.h  \
						src/Santiago/Fastcgi/Request.h \
						src/Santiago/Fastcgi/RequestData.h

fastcgi_devkit_HEADERS = 			src/Santiago/fastcgi_devkit/fastcgi.h

SantiagoDBTables_HEADERS = \
						src/Santiago/SantiagoDBTables/DatabaseRecords.h \
						src/Santiago/SantiagoDBTables/Definitions.h \
						src/Santiago/SantiagoDBTables/MariaDBConnection.h

ThreadSpecificVar_HEADERS = 			src/Santiago/ThreadSpecificVar/ThreadSpecificVar.h

User_HEADERS =					src/Santiago/User/ControllerBase.h

SingleNode_HEADERS =         			src/Santiago/User/SingleNode/Controller.h

Utils_HEADERS = 				src/Santiago/Utils/PTimeUtils.h \
						src/Santiago/Utils/STLog.h 
						
 

# the sources to add to the library and to add to the source distribution
libSantiago_a_SOURCES =	\
						$(Santiago_a_HEADERS) \
						$(AppServer_HEADERS) \
						$(Fastcgi_HEADERS)\
						$(fastcgi_devkit_HEADERS)\
						$(SantiagoDBTables_HEADERS)\
						$(ThreadSpecificVar_HEADERS)\
						$(User_HEADERS)\
						$(SingleNode_HEADERS)\
						$(Utils_HEADERS)\
						src/Santiago/ErrorCategory.cpp \
						src/Santiago/HTTPDefinitions.cpp \
						src/Santiago/SantiagoDBTables/Definitions.cpp \
						src/Santiago/SantiagoDBTables/MariaDBConnection.cpp \
						src/Santiago/User/SingleNode/Controller.cpp \
						src/Santiago/Utils/PTimeUtils.cpp \
						src/Santiago/Utils/STLog.cpp
						
						
			 



