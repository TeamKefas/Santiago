Santiago
========

Santiago project is aimed at creating a C++ based web application framework.

Currently it only implements the fastcgi protocol connector and a minimal application server. Its boost asio based asynchronous request handling system is designed to solve the c10k problem. It is designed to enable easy creation of new asynchronous services(like authentication service,database service etc.). It exposes the underlying boost asio api in a safe manner so as to enable users to use the existing asio facilities to create these services.  

If you have any feedback or find this useful please send me a mail at wil.pazhorickal@gmail.com. It would be a great encouragement for me to continue on the project.

Tasks
-----
Make fastcgi library - completed.
Make minimal asynchronous server - completed.
Make mysql service.
Make redis service.
Make templating engine/integrate with existing templating engine.
Make main controller.
Make synchronous variant.

How to Compile
--------------
This is a header only library. But it needs to link to boost thread and boost system.

To compile the test cases
create the build directory for cmake and cd to that directory

cmake <path to santiago root>

make

This should produce FastcgiEcho, ServerEcho and AsyncServerEcho in the <path to santiago root>/testbin directory.

Test Cases
----------
3 test/sample programs are given. The nginx conf files for the test cases are given in the test directory. The test urls mentioned are assuming this configuration.

a) Fastcgi tcp echo: Tests the tcp socket connector to the fastcgi. The program opens a tcp socket at port 7000 and listens for requests. Test url for the given nginx.conf is http://localhost:8080/fastcgi-tcp-echo.fcgi .

b) Fastcgi unix echo: Tests the unix socket connector to the fastcgi. The program creates a unix socket /tmp/fastcgi-unix-echo.socket and listens for requests. Test url for the given nginx.conf is http://localhost:8080/fastcgi-unix-echo.fcgi .

c) Server tcp echo A and B: Tests the minimal server. The program listens at the tcp port 7001. Test urls are http://localhost:8080/server-tcp-echoA.fcgi and http://localhost:8080/server-tcp-echoB.fcgi .

d) Server async timer service: Makes a test timer service using the underlying boost asio ioservice. The program listesn to port 7002. Test url is http://localhost:8080/async-service-timer-echoA.fcgi . For the cancel test(simply returns error) url is http://localhost:8080/async-service-cancel.fcgi . 

License
-------
Copyright 2012, 2013 Wilson Sunny Philip [wil.pazhorickal@gmail.com]

Santiago is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Santiago is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Santiago.  If not, see <http://www.gnu.org/licenses/>.
