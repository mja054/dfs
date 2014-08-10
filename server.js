var fs = require('fs')
var http = require('http')
var strftime = require('strftime')

var i = 0;
var path = "/root/configure/";
var xivelyBackendFile = path + "xively.txt"
var keys = ["id", "description", "position", "ip", "rssi"]
function send(res, header)
{
    res.writeHead(200, "OK", header);
    res.write("Hello World");
    res.end();
}

function parse_header(header,des)
{
    for (var i = 0; i < keys.length; i++)
        des[keys[i]] = header[keys[i]];
}

var server = http.createServer(function(req, res) {

    var flag = 0;
    var data = {}
//  console.log(req.method);
//  console.log(req.headers);
    console.log("IP: " + req.connection.remoteAddress);
    file = path + '/' + req.headers['id']
    if (req.method == 'GET') {
        console.log("GET Method");
        flag = 1;
        fs.exists(file, function(exists) {
           if (exists) {
               fs.stat(file, function(error, stats) {
                   fs.readFile(file, 'utf-8', function(err, data){
                       console.log(JSON.parse(data));
                        send(res, JSON.parse(data));
                   });
                });
           } else {
                data['info'] = "IBeacon not registered";
               console.log("file doesn't exists" + data);
               send(res, data);
           }
       });
    } else if (req.method == 'PUT') {
        console.log("PUT Method "+ file + ", " + req.headers['ip']);
        parse_header(req.headers, data);
        if (req.headers['ip'])
                fs.link(file, path+req.headers['ip'], function(err) {});
        fs.writeFile(file, JSON.stringify(data, null, 4), function(err) {
        });
        send(res, data);
    } else if (req.method == 'POST') {
        console.log("POST Method "+ file);
        var data = {}
        parse_header(req.headers, data);
        line = data["id"] + ',' + data["rssi"] + ',' + strftime('%Y-%m-%dT%H:%M:%S') + '-08:00\n';
        //console.log(line);
        fs.appendFile(xivelyBackendFile, line, function(err) {
            if (err)
                console.log("error while appendingFile");
        });
        send(res, data);
    }
}).listen(80, '128.97.93.163');
