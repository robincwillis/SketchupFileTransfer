#define CURL_STATICLIB

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <curl/multi.h>
#include <curl/easy.h>
#include <ruby.h>


/* -----------------------------------------------------------------------------
    PROTOTYPES
----------------------------------------------------------------------------- */

// Variable prototypes. This stores the Ruby class information that we will
// bind the download_file and download_files methods to in the FileDownloader 
// class.
static VALUE ConnectionManager; // = Qnil;
static VALUE Download;
static VALUE Upload;
// Class prototype:
void Init_ConnectionManager();

/* -----------------------------------------------------------------------------
    METHODS
----------------------------------------------------------------------------- */
struct string {
  char *ptr;
  size_t len;
};

static void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

static size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}


// Download a single file supplied by a Ruby String URL and a filesystem path to
// store the downloaded file using certificate checking
static VALUE download_file(VALUE self, VALUE rs_url, VALUE rs_path, VALUE rs_auth, VALUE rs_cert)
{
    
    // Make sure the method arguments are Ruby Strings.
    Check_Type(rs_url, T_STRING);
    Check_Type(rs_path, T_STRING);
	Check_Type(rs_auth, T_STRING);
    Check_Type(rs_cert, T_STRING);
    // Initialize cURL variables.
    CURL *curl;
    FILE *fp;
    CURLcode res;
    long http_code = 0;
	
    // Initialize cURL.
    curl = curl_easy_init();
    
    // Make sure cURL is initialized.
    if (curl) {
    
        // Convert the Ruby URL and path strings into C strings so we can 
        // pass it to cURL.
        char *cs_url, *cs_path, *cs_auth, *cs_cert;
        cs_url = StringValueCStr(rs_url);
        cs_path = StringValueCStr(rs_path);
		cs_auth = StringValueCStr(rs_auth);
		cs_cert = StringValueCStr(rs_cert);
        
        // Set general cURL options.
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); // For debugging...
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL); //write_data
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
        
		//Setup SSL and Certificate verification
		curl_easy_setopt (curl, CURLOPT_SSL_VERIFYPEER, 1); 
		curl_easy_setopt (curl, CURLOPT_CAINFO, cs_cert);
		
		//Setup Authentication
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(curl, CURLOPT_USERPWD, cs_auth);
		
        // Tell cURL the URL to the file that we want downloaded.
        curl_easy_setopt(curl, CURLOPT_URL, cs_url);
        
        // Open a file for writing and tell cURL to write to it.
        fp = fopen(cs_path, "wb");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        
        // Perform the download.
        res = curl_easy_perform(curl);
        
		//Get the response code from the server
		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
		
        // Clean up the cURL session.
        curl_easy_cleanup(curl);
        
        // Close the file after it has been written to.
        fclose(fp);
        
    }
    
	//Return true or false to Ruby based on curl and response code so the function can check whether or not it succeeded
	if (http_code != 200 || res != 0)
	{
		return Qfalse;
	}
	return Qtrue;
    
}

// Upload a single file supplied by a Ruby String URL and a filesystem path to a server by
//building a mulitpart/data form using basic authentication and certificate checking
static VALUE upload_file(VALUE self, VALUE rs_url, VALUE rs_path, VALUE rs_auth, VALUE rs_cert)
{

	// Make sure the method arguments are Ruby Strings.
    Check_Type(rs_url, T_STRING);
    Check_Type(rs_path, T_STRING);
    Check_Type(rs_auth, T_STRING);
	Check_Type(rs_cert, T_STRING);
 
    // Initialize cURL variables.
    CURL *curl;
    CURLcode res;
    long http_code = 0;
    struct string s;
    init_string(&s);

    // Initialize cURL.
    curl = curl_easy_init();
    
    // Make sure cURL is initialized.
    if (curl) {
	
		// Convert the Ruby URL and path strings into C strings so we can 
	    // pass it to cURL.
	    char *cs_url, *cs_path, *cs_auth, *cs_cert;
	    cs_url = StringValueCStr(rs_url);
		cs_path = StringValueCStr(rs_path);
		cs_auth = StringValueCStr(rs_auth);
		cs_cert = StringValueCStr(rs_cert);
		
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // For debugging...
		
		//Set the URL for the file upload
		curl_easy_setopt(curl, CURLOPT_URL, cs_url); 
		
		//Setup SSL and Certificate verification
		curl_easy_setopt (curl, CURLOPT_SSL_VERIFYPEER, 1); 
		curl_easy_setopt (curl, CURLOPT_CAINFO, cs_cert);
		
		//Setup Authentication
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(curl, CURLOPT_USERPWD, cs_auth);
		
		//Setup How to Write the Response
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
	    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

		//Setup Multi-part form post
		struct curl_httppost *post = NULL;
		struct curl_httppost *last = NULL;
		
		//add file to form
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "myFile", CURLFORM_FILE, cs_path, CURLFORM_END);
		
		
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
		
		//Perform the upload
		res = curl_easy_perform(curl);
		
		//Get the response code from the server
		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
		
		//free a previously build multipart/formdata
		curl_formfree(post);
		
		//curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
		
		//clean up the cURL session
		curl_easy_cleanup(curl);
		
	}
	
	//Return response from server or nil
	if (http_code != 200 || res != 0)
	{
		return Qnil;
	}
	return rb_str_new2(s.ptr);
}

static VALUE upload_files(VALUE self, VALUE rs_url, VALUE ra_files, VALUE rs_auth, VALUE rs_cert)
{
	//Make sure everything passed is the right type
	Check_Type(ra_files, T_ARRAY);
	Check_Type(rs_url, T_STRING);
    Check_Type(rs_auth, T_STRING);
	Check_Type(rs_cert, T_STRING);
	
	// Initialize cURL variables.
    CURL *curl;
    CURLcode res;
    long http_code = 0;
    struct string s;
    init_string(&s);

    // Initialize cURL.
    curl = curl_easy_init();
    
    // Make sure cURL is initialized.
    if (curl) {
		
		
		// Convert the Ruby URL and path strings into C strings so we can 
	    // pass it to cURL.
	    char *cs_url, *cs_auth, *cs_cert;
	    cs_url = StringValueCStr(rs_url);
		cs_auth = StringValueCStr(rs_auth);
		cs_cert = StringValueCStr(rs_cert);
		
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // For debugging...
		
		//Set the URL for the file upload
		curl_easy_setopt(curl, CURLOPT_URL, cs_url); 
		
		//Setup SSL and Certificate verification
		curl_easy_setopt (curl, CURLOPT_SSL_VERIFYPEER, 1); 
		curl_easy_setopt (curl, CURLOPT_CAINFO, cs_cert);
		
		//Setup Authentication (might not be necessary for download)
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(curl, CURLOPT_USERPWD, cs_auth);
		
		

		//Setup Multi-part form post
		struct curl_httppost *post = NULL;
		struct curl_httppost *last = NULL;
		
		unsigned long int num_files = RARRAY(ra_files)->len;
		long int i;
	
		for (i = 0; i < num_files; ++i){
		
			// Initialize and get this item in the Ruby array.
			VALUE ra_item;
			ra_item = rb_ary_entry(ra_files, i);
			Check_Type(ra_item, T_ARRAY);
		
			//get key and path from array
			VALUE rs_key, rs_path;
			rs_key = rb_ary_entry(ra_item, 0);
			rs_path = rb_ary_entry(ra_item, 1);
		
			//convert to c strings
			char *cs_key, *cs_path;
			cs_key = StringValueCStr(rs_key);
			cs_path = StringValueCStr(rs_path);
			//add the form
			curl_formadd(&post, &last, CURLFORM_COPYNAME, cs_key, CURLFORM_FILE, cs_path, CURLFORM_END);
	
		}
	
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
		
		//Perform the upload
		res = curl_easy_perform(curl);
		
		//Get the response code from the server
		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
		
		//free a previously build multipart/formdata
		curl_formfree(post);
		
		//curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
		
		//clean up the cURL session
		curl_easy_cleanup(curl);
	
	}
	
	//Return response from server or nil
	if (http_code != 200 || res != 0)
	{
		return Qnil;
	}
	return rb_str_new2(s.ptr);
	
}

static VALUE upload_files_with_commit_message(VALUE self, VALUE rs_url, VALUE rs_message, VALUE ra_files, VALUE rs_auth, VALUE rs_cert)
{
	//Make sure everything passed is the right type
	Check_Type(ra_files, T_ARRAY);
	Check_Type(rs_url, T_STRING);
	Check_Type(rs_message, T_STRING);
    Check_Type(rs_auth, T_STRING);
	Check_Type(rs_cert, T_STRING);
	
	// Initialize cURL variables.
    CURL *curl;
    CURLcode res;
    long http_code = 0;
    struct string s;
    init_string(&s);

    // Initialize cURL.
    curl = curl_easy_init();
    
    // Make sure cURL is initialized.
    if (curl) {
		
		
		// Convert the Ruby URL and path strings into C strings so we can 
	    // pass it to cURL.
	    char *cs_url, *cs_message, *cs_auth, *cs_cert;
	    cs_url = StringValueCStr(rs_url);
		cs_message = StringValueCStr(rs_message);
		cs_auth = StringValueCStr(rs_auth);
		cs_cert = StringValueCStr(rs_cert);
		
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // For debugging...
		
		//Set the URL for the file upload
		curl_easy_setopt(curl, CURLOPT_URL, cs_url); 
		
		//Setup SSL and Certificate verification
		curl_easy_setopt (curl, CURLOPT_SSL_VERIFYPEER, 1); 
		curl_easy_setopt (curl, CURLOPT_CAINFO, cs_cert);
		
		//Setup Authentication (might not be necessary for download)
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(curl, CURLOPT_USERPWD, cs_auth);
		
		//Setup How to Write the Response
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
	    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

		//Setup Multi-part form post
		struct curl_httppost *post = NULL;
		struct curl_httppost *last = NULL;
		
		unsigned long int num_files = RARRAY(ra_files)->len;
		long int i;
	
		for (i = 0; i < num_files; ++i){
		
			// Initialize and get this item in the Ruby array.
			VALUE ra_item;
			ra_item = rb_ary_entry(ra_files, i);
			Check_Type(ra_item, T_ARRAY);
		
			//get key and path from array
			VALUE rs_key, rs_path;
			rs_key = rb_ary_entry(ra_item, 0);
			rs_path = rb_ary_entry(ra_item, 1);
		
			//convert to c strings
			char *cs_key, *cs_path;
			cs_key = StringValueCStr(rs_key);
			cs_path = StringValueCStr(rs_path);
			//add the form
			curl_formadd(&post, &last, CURLFORM_COPYNAME, cs_key, CURLFORM_FILE, cs_path, CURLFORM_END);
	
		}
		
		//add the commit message
		
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "message", CURLFORM_COPYCONTENTS, cs_message, CURLFORM_END);
		
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
		
		//Perform the upload
		res = curl_easy_perform(curl);
		
		//Get the response code from the server
		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
		
		//free a previously build multipart/formdata
		curl_formfree(post);
		
		//curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
		
		//clean up the cURL session
		curl_easy_cleanup(curl);
	
	}
	
	//Return response from server or nil
	if (http_code != 200 || res != 0)
	{
		return Qnil;
	}
	return rb_str_new2(s.ptr);
	
}

/* -----------------------------------------------------------------------------
    CLASSSES
----------------------------------------------------------------------------- */

// This function constructs a Ruby Class and adds the downloading/uploading methods to it.
void Init_ConnectionManager()
{
    
    ConnectionManager = rb_define_module("ConnectionManager");
    
	// > ConnectionManager::VERSION
	rb_define_const(ConnectionManager, "VERSION", rb_str_new2("1.0.0"));
	
	Download = rb_define_class_under(ConnectionManager, "Download", rb_cObject);
	
	Upload = rb_define_class_under(ConnectionManager, "Upload", rb_cObject);
    
    // > ConnectionManager::Download.download_file(url, path, cert_path)
    rb_define_method(Download, "download_file", download_file, 4);

	// > ConnectionManager::Upload.upload_file(url, path, auth, cert_path)
    rb_define_method(Upload, "upload_file", upload_file, 4);
	
	// > ConnectionManager::Upload.upload_files(url, files, auth, cert_path)
	rb_define_method(Upload, "upload_files", upload_files, 4);

    // > ConnectionManager::Upload.upload_files(url, files, auth, cert_path)
	rb_define_method(Upload, "upload_files_with_commit_message", upload_files_with_commit_message, 5);
}


