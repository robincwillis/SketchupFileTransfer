SketchupFileTransfer
====================

Utility that allows Sketchup plugins to upload and download files through http requests using cURL


Features
-----
- Download individual file
- Upload individual file
- Upload multiple files
- Upload multiple files with a message
- Supports Basic Authentication

Setup
-----
```
> cd ConMgr
> make clean
> ruby extconf.rb
> make
```

How To Use
-----
Require and create an instance of the ConnectionManager
```ruby
require 'ConnectionManager'

dl = ConnectionManager::Download.new
up = ConnectionManager::Upload.new
```

Set required variables
```ruby
#a base64 encoded string for basic authentication
auth = 'token'
#the api endpoint to post the file/files to or the location of the file to download
url = 'https://your-site.com'
#list of valid certificates necessary to prevent security issues in windows
cert_path = File.join(base_path, "cacert.pem")
#lets pick a place to upload and download files from
base_path = File.expand_path(File.dirname(__FILE__))
```
Download a single file
```ruby

dl_path = File.join(base_path, "test.obj")

if dl.download_file(url, dl_path, auth, cert)
   puts "\tFile downloaded successfully!"
else
 	puts "\tFile failed to download!!!"
end
```
Upload a single file
```ruby

file_path = File.join(base_path, "box.obj")

res = up.upload_file(url, file_path, auth, cert_path)
if res.nil?
  puts "\tFile failed to upload!!!"
else
	puts res    
end
```
Upload multiple files
```ruby
other_path = File.join(base_path, "box.obj.mtl")

files = [
    ["file",file_path],
    ["other",other_path]
  ]

res = up.upload_files(url, files, auth, cert_path)
   puts "\tFile uploaded successfully!"
 else
   puts res
 end
```

Also
----
util/deploySketchup.sh
Also included is a handy utility script that I like to use to build my sketchup plugins that came out of complications from designing plugins in a way that host their UI on a remote server.
