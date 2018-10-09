var dns = require('dns');
var w3 = dns.lookup('w3schools.com', function (err, addresses, family) {
  console.log(addresses);
});

var fs = require('fs');


var read_file = function(filename){
	fs.readFile(filename, 'utf8', function(err, data) {
		if (err){
			//throw err;
			console.log("ERROR: " + err);
			return;
		}
		console.log(data);
	});
}

read_file("/boot/home/README_FAIL");
read_file("/boot/home/README");
