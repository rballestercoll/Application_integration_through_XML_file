# Application_integration_through_XML_file
> Small application capable of extracting certain information from various machines on the network, generating an XML document with it.

## Objective
Console application written in C, the functionality of which will be to generate an XML file from the following data of the network adapters of the equipment and the one indicated by the user of the application: the IP, mask, gateway, the primary DNS server, the average speed of response of this DNS server and the number of hops to which this DNS server is also detailing the IP of each hop. 

## Route
1. We start the program and a navigation menu will appear that will allow us to launch only two options, Initialize the program, or exit it.
2. When the program starts, a list of all the adapters installed in the system will appear on the screen, indicating which are functional and which are not. Then it asks us which adapter we want to work with.
3. We write the name of the adapter correctly, and the program starts to generate temporary files in which it will save the necessary information to be printed on the screen, the IP, the mask, the gateway, the Primary DNS server, the average speed of response of the DNS server and the number of hops at which this DNS server is located. DNS server and the number of hops at which this DNS server is located.
4. Finally it warns us that it has finished the search and read process and that it has successfully generated the XML document and deleted the temporary files.