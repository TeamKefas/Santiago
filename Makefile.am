lib_LIBRARIES = libSantiago.a

AM_CPPFLAGS = -I$(top_srcdir)/src				
# where to install the headers on the system
Santiago_adir = $(includedir)/Santiago
AppServerdir = $(includedir)/Santiago/AppServer
Fastcgidir = $(includedir)/Santiago/Fastcgi
fastcgi_devkitdir = $(includedir)/Santiago/fastcgi_devkit
Utilsdir = $(includedir)/Santiago/Utils
Threaddir = $(includedir)/Santiago/Thread
SantiagoDBTablesdir = $(includedir)/Santiago/SantiagoDBTables
Authenticationdir = $(includedir)/Santiago/Authentication
SingleNodedir = $(includedir)/Santiago/Authentication/SingleNode


# the list of header files that belong to the library (to be installed later)
Santiago_a_HEADERS =	\
						$(top_srcdir)/src/Santiago/ErrorCategory.h  \
						$(top_srcdir)/src/Santiago/HTTPDefinitions.h 

AppServer_HEADERS = \
		       				$(top_srcdir)/src/Santiago/AppServer/Request.h \
		      				$(top_srcdir)/src/Santiago/AppServer/RequestHandlerBase.h \
		     			        $(top_srcdir)/src/Santiago/AppServer/ServerBase.h
 
Fastcgi_HEADERS = \
						$(top_srcdir)/src/Santiago/Fastcgi/Acceptor.h \
						$(top_srcdir)/src/Santiago/Fastcgi/Common.h \
						$(top_srcdir)/src/Santiago/Fastcgi/Connection.h \
						$(top_srcdir)/src/Santiago/Fastcgi/ConnectionData.h \
						$(top_srcdir)/src/Santiago/Fastcgi/PacketSocket.h \
						$(top_srcdir)/src/Santiago/Fastcgi/PairParseFunctions.h \
						$(top_srcdir)/src/Santiago/Fastcgi/RecordSocket.h  \
						$(top_srcdir)/src/Santiago/Fastcgi/Request.h \
						$(top_srcdir)/src/Santiago/Fastcgi/RequestData.h

fastcgi_devkit_HEADERS = 			$(top_srcdir)/src/Santiago/fastcgi_devkit/fastcgi.h

Thread_HEADERS = 				$(top_srcdir)/src/Santiago/Thread/ThreadSpecificVar.h \
						$(top_srcdir)/src/Santiago/Thread/ThreadSpecificVarImpl.h 

SantiagoDBTables_HEADERS = \
						$(top_srcdir)/src/Santiago/SantiagoDBTables/DatabaseRecords.h \
						$(top_srcdir)/src/Santiago/SantiagoDBTables/Definitions.h \
						$(top_srcdir)/src/Santiago/SantiagoDBTables/MariaDBConnection.h

Authentication_HEADERS = 			$(top_srcdir)/src/Santiago/Authentication/AuthenticatorBase.h

SingleNode_HEADERS =         			$(top_srcdir)/src/Santiago/Authentication/SingleNode/Authenticator.h


Utils_HEADERS = 				$(top_srcdir)/src/Santiago/Utils/PTimeUtils.h \
						$(top_srcdir)/src/Santiago/Utils/STLog.h \
						$(top_srcdir)/src/Santiago/Utils/Email.h 
						
 

# the sources to add to the library and to add to the source distribution
libSantiago_a_SOURCES =	\
						$(Santiago_a_HEADERS) \
						$(AppServer_HEADERS) \
						$(Fastcgi_HEADERS)\
						$(fastcgi_devkit_HEADERS)\
						$(Utils_HEADERS)\
						$(Thread_HEADERS)\
						$(SantiagoDBTables_HEADERS)\
						$(Authentication_HEADERS)\
						$(SingleNode_HEADERS)\
						src/Santiago/ErrorCategory.cpp \
						src/Santiago/HTTPDefinitions.cpp \
						src/Santiago/Utils/PTimeUtils.cpp \
						src/Santiago/Utils/STLog.cpp \
						src/Santiago/Utils/Email.cpp \
						src/Santiago/Thread/ThreadSpecificVarImpl.cpp \
						src/Santiago/SantiagoDBTables/Definitions.cpp \
						src/Santiago/SantiagoDBTables/MariaDBConnection.cpp \
						src/Santiago/Authentication/AuthenticatorBase.cpp \
						src/Santiago/Authentication/SingleNode/Authenticator.cpp 
						
						
			 



