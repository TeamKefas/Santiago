lib_LIBRARIES = libSantiago.a
						
# where to install the headers on the system
Santiago_adir = $(includedir)/Santiago
Authenticationdir = S(includedir)/Santiago/Authentication
SingleNodedir = $(includedir)/Santiago/Authentication/SingleNode
AppServerdir = $(includedir)/Santiago/AppServer
Fastcgidir = $(includedir)/Santiago/Fastcgi
fastcgi_devkitdir = $(includedir)/Santiago/fastcgi_devkit
SantiagoDBTablesdir = $(includedir)/Santiago/SantiagoDBTables
Threaddir = $(includedir)/Santiago/Thread
Utilsdir = $(includedir)/Santiago/Utils

# the list of header files that belong to the library (to be installed later)
Santiago_a_HEADERS =	\
						src/Santiago/ErrorCategory.h  \
						src/Santiago/HTTPDefinitions.h \
						src/Santiago/LoclEndPoint.h

Authentication_HEADERS = 			src/Santiago/Authentication/AuthenticatorBase.h

SingleNode_HEADERS =         			src/Santiago/Authentication/SingleNode/Authenticator.h 

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

Thread_HEADERS = 				src/Santiago/Thread/ThreadSpecificVar.h \
						src/Santiago/Thread/ThreadSpecificVarImpl.h


Utils_HEADERS = 				src/Santiago/Utils/PTimeUtils.h \
						src/Santiago/Utils/STLog.h \
						src/Santiago/Utils/Email.h 
						
 

# the sources to add to the library and to add to the source distribution
libSantiago_a_SOURCES =	\
						$(Santiago_a_HEADERS) \
						$(Authentication_HEADERS)\
						$(SingleNode_HEADERS)\
						$(AppServer_HEADERS) \
						$(Fastcgi_HEADERS)\
						$(fastcgi_devkit_HEADERS)\
						$(SantiagoDBTables_HEADERS)\
						$(Thread_HEADERS)\
						$(Utils_HEADERS)\
						src/Santiago/ErrorCategory.cpp \
						src/Santiago/HTTPDefinitions.cpp \
						src/Santiago/Authentication/AuthenticatorBase.cpp \
						src/Santiago/Authentication/SingleNode/Authenticator.cpp \
						src/Santiago/SantiagoDBTables/Definitions.cpp \
						src/Santiago/SantiagoDBTables/MariaDBConnection.cpp \
						src/Santiago/Thread/ThreadSpecificVarImpl.cpp \
						src/Santiago/Utils/PTimeUtils.cpp \
						src/Santiago/Utils/STLog.cpp \
						src/Santiago/Utils/Email.cpp
						
						
			 



