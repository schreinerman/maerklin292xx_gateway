import os

strInitScript = "WebServer* _pServer;\r\n"
strInitScript += "void HtmlFs_Init(WebServer* pServer)\r\n{\r\n"
strInitScript += "    _pServer = pServer;\r\n"
strInitScript += "    _pServer->on(\"/\", []() {\r\n"
strInitScript += "        _pServer->send(200, \"text/html\", (char*)au8index_html);\r\n"
strInitScript += "    });\r\n"
htmlFsFile = open('htmlfs.cpp', 'w') 
htmlFsFile.write("#include \"htmlfs.h\"\r\n")
htmlFsFile.write("#include <stdint.h>\r\n")
htmlFsFile.write("#include <WebServer.h>\r\n")

for subdir, dirs, files in os.walk("html"):
    for filename in files:
        filepath = subdir + os.sep + filename
        if (filename != ".DS_Store"):
            fileName = filepath

            fileType = "text/plain"
            if fileName.endswith("html"):
                fileType = "text/html"
            if fileName.endswith("png"):
                fileType = "image/png"
            if fileName.endswith("jpg"):
                fileType = "image/jpeg"

            strFileName = fileName[5:]
            strInitScript += "    _pServer->on(\"/" + strFileName + "\", []() {\r\n"
            strFileName = strFileName.replace("/","_")
            strFileName = strFileName.replace(".","_")
            if (fileType.startswith("text")):
                strInitScript += "        _pServer->send(200, \"" + fileType + "\", (char*)au8" + strFileName + ");\r\n"
            else:
                fileSize = os.path.getsize(fileName)
                strInitScript += "        _pServer->send_P(200, \"" + fileType + "\", (char*)au8" + strFileName + "," + str(fileSize) + ");\r\n"
            strInitScript += "    });\r\n"

            if (fileType.startswith("text")):
                readFile = open(fileName, 'r') 
                Lines = readFile.readlines() 
                readFile.close()
                htmlFsFile.write("const char au8" + strFileName + "[] = ")


                count = 0
                # Strips the newline character 
                for line in Lines: 
                    lineTmp = line.replace("\"","\\\"")
                    lineTmp = lineTmp.replace("\r\n","")
                    lineTmp = lineTmp.replace("\n","")
                    lineTmp = "\"" + lineTmp + "\\r\\n\"\r\n"
                    htmlFsFile.write(lineTmp)

                htmlFsFile.write(";\r\n")
            else:
                htmlFsFile.write("const uint8_t au8" + strFileName + "[" + str(fileSize) + "] = {")
                readBinFile = open(fileName, 'rb');
                byte = readBinFile.read(1)
                while byte:
                    htmlFsFile.write(hex(bytearray(byte)[0]) + ", ")
                    byte = readBinFile.read(1)
                htmlFsFile.write("};\r\n")    

strInitScript += "}\r\n"

htmlFsFile.write("\r\n" + strInitScript);


htmlFsFile.close()
