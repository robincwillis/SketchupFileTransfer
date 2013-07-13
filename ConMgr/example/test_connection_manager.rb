require 'ConnectionManager'

dl_1 = ConnectionManager::Download.new

up_1 = ConnectionManager::Upload.new

auth = ''

url = ''

file_name = ''

base_path = File.expand_path(File.dirname(__FILE__))

path = File.join(base_path, file_name)


if up_1.upload_file(url, path, auth)
    puts "\tFile uploaded successfully!"
  else
    puts "\tFile failed to upload!!!"
  end


if dl_1.download_file(url, path)
  puts "\tFile downloaded successfully!"
else
  puts "\tFile failed to download!!!"
end