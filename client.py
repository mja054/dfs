import urllib2
import json

SERVER_URL = 'http://128.97.93.163:8080'
#SERVER_URL = 'http://127.0.0.1:8080'

"""
Creates the connection object based on requestCode,
requestName and the body.
Body is a json encoded string.
Makes the request and returns the response.
"""
def makeRequest(requestCode, requestName, body=None):
    headers = {'Content-type':'application/json'}
    request = urllib2.Request(SERVER_URL, body, headers)
    
    """
    encode json like this
    foo = {'text' : 'Hello World github/linguist#1 **cool**, and #1!' }
    json_foo = json.dumps(foo)
    """
    #HTTPConnection.request(method, url[, body[, headers]])
    #connection.request('POST', '', body,headers)
    response = urllib2.urlopen(request)
    #For debugging
    print(response.read().decode()) 
    return response


def getUsers():
    requestCode = 101
    requestName = 'GET_USERS'
    response = makeRequest(requestCode, requestName)
    #TODO - process response


def addSelf():
    requestCode = 102
    requestName = 'ADD_SELF'
    response = makeRequest(requestCode, requestName)
    #TODO - process response


def getUserInfo(userId):
    requestCode = 103
    requestName = 'GET_INFO'
    pathObj = {'userId' : userId}
    pathJson = json.dumps(pathObj)
    response = makeRequest(requestCode, requestName)
    #TODO - process response

def listDir(path):
    requestCode = 201
    requestName = 'LIST_DIR'
    pathObj = {'path' : path}
    pathJson = json.dumps(pathObj)
    response = makeRequest(requestCode, requestName, pathJson)
    #TODO - process response

def listContent(path):
    requestCode = 102
    requestName = 'ADD_SELF'
    pathObj = {'path' : path}
    pathJson = json.dumps(pathObj)
    response = makeRequest(requestCode, requestName, pathJson)
    #TODO - process response

getUsers()
