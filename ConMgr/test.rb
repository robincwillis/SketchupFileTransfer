
require 'ConnectionManager'

dl_1 = ConnectionManager::Download.new
up_1 = ConnectionManager::Upload.new



auth = 'AChoAAVdthzr7s:K9bEJ%2FVZ61e9zn'
url = 'https://sunglass.io/api/v1.2/projects/12284/models'
#path = '/Users/rudolph/Desktop/pink-test.obj'


base_path = File.expand_path(File.dirname(__FILE__))
cert_path = File.join(base_path, "cacert.pem")
model_path = File.join(base_path, "box.obj")
material_path = File.join(base_path, "box.obj.mtl")

files = [
    ["model",model_path],
    ["material",material_path]
  ]


res = up_1.upload_file(url, model_path, auth, cert_path)
if res.nil?
    puts "no dice"
  else
	puts res    
  end


#if up_1.upload_files(url, files, auth, cert_path)
#    puts "\tFile uploaded successfully!"
#  else
#    puts "\tFile failed to upload!!!"
#  end


#if dl_1.download_file(url, path, auth, cert)
#  puts "\tFile downloaded successfully!"
#else
#  puts "\tFile failed to download!!!"
#end
