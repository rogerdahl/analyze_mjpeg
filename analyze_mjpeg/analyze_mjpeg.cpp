#include "stdafx.h"

#include "jpeg.h"
#include "display.h"
#include "filter_average.h"
#include "filter_nightvision.h"


using boost::asio::ip::tcp;
using namespace std;
using namespace std::tr1;

using boost::lexical_cast;

int run(int argc, char* argv[]);


int main(int argc, char* argv[])
{
  if (argc != 4) {
    cout << "Usage: " << argv[0] << " <server> <port> <path>\n" << endl;
    return 1;
  }

  try {
    return run(argc, argv);
  }
  catch (exception& e) {
    cout << "Exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}


int run(int argc, char* argv[])
{
  DisplayImage display_image;

  //FilterAverage filter;
  FilterNightVision filter;

  boost::asio::io_service io_service;

  // Get a list of endpoints corresponding to the server name.
  tcp::resolver resolver(io_service);
  tcp::resolver::query query(argv[1], argv[2]);
  tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
  tcp::resolver::iterator end;

  // Try each endpoint until we successfully establish a connection.
  tcp::socket socket(io_service);
  boost::system::error_code error = boost::asio::error::host_not_found;
  while (error && endpoint_iterator != end) {
    socket.close();
    socket.connect(*endpoint_iterator++, error);
  }
  if (error) {
    throw boost::system::system_error(error);
  }

  // Form the request. We specify the "Connection: close" header so that the
  // server will close the socket after transmitting the response. This will
  // allow us to treat all data up until the EOF as the content.
  boost::asio::streambuf request;
  ostream request_stream(&request);
  request_stream << "GET " << argv[3] << " HTTP/1.0\r\n";
  request_stream << "Host: " << argv[1] << "\r\n";
  request_stream << "Accept: */*\r\n";
  request_stream << "Connection: close\r\n\r\n";

  // Send the request.
  boost::asio::write(socket, request);

  // Read the response status line. The response streambuf will automatically
  // grow to accommodate the entire line. The growth may be limited by passing
  // a maximum size to the streambuf constructor.
  boost::asio::streambuf response;
  boost::asio::read_until(socket, response, "\r\n");

  // Check that response is OK.
  istream response_stream(&response);
  string http_version;
  response_stream >> http_version;
  unsigned int status_code;
  response_stream >> status_code;
  string status_message;
  getline(response_stream, status_message);
  if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
    cout << "Invalid response\n";
    return 1;
  }
  if (status_code != 200) {
    cout << "Response returned with status code " << status_code << "\n";
    return 1;
  }

  // Read the response headers, which are terminated by a blank line.
  boost::asio::read_until(socket, response, "\r\n\r\n");

  // Get the MIME multipart boundary from the headers.
  regex rx_content_type("Content-Type:.*boundary=(.*)");
  regex rx_content_length("Content-Length: (.*)");

  smatch match;
  string header;
  string boundary;

  while (getline(response_stream, header) && header != "\r") {
    //cout << "HTTP HEADER: " << header << endl;
    if (regex_search(header, match, rx_content_type)) {
      boundary = match[1];
      //cout << "BOUNDARY SELECTED: " << boundary << endl;
    }
  }

  // Abort if a boundary was not found.
  if (boundary == "") {
    cout << "Not a MJPEG stream" << endl;
    return false;
  }

  u32 buf_size(0);
  char* buf(0);

  while(1) {
    //cout << "-- START CYCLE --" << endl;

    //cout << "RESPONSE SIZE, BEFORE READ TO BOUNDARY: " << response.size() << endl;

    // Read until there is a boundary in the response. If there is already a
    // boundary in the buffer, this is a no-op.
    boost::asio::read_until(socket, response, boundary);

    //cout << "RESPONSE SIZE, AFTER READ TO BOUNDARY: " << response.size() << endl;

    // Remove everything up to and including the boundary that is now known to
    // be there.
    while (getline(response_stream, header)) {
      //cout << "BOUNDARY SEARCH: " << header << endl;
      if (header == boundary + "\r") {
        //cout << "BOUNDARY FOUND: " << header << endl;
        break;
      }
    }

    // Read the headers that follow the boundary. These always end with a blank
    // line. Content-Length must be one of the header lines, and the size of the
    // compressed jpeg is read from it.

    //cout << "RESPONSE SIZE, AFTER BOUNDARY SEARCH: " << response.size() << endl;

    u32 content_length;
    while (getline(response_stream, header) && header != "\r") {
      //cout << "MM HEADER: " << header << endl;
      if (regex_search(header, match, rx_content_length)) {
        content_length = lexical_cast<int>(match[1]);
        //cout << "MM HEADER CONTENT-LENGTH FOUND: " << content_length << endl;
      }
    }
    
    // Read until the entire jpeg is in the response.
    if (response.size() < content_length) {
      boost::asio::read(socket, response, boost::asio::transfer_at_least(
        content_length - response.size()));
    }

    //cout << "RESPONSE SIZE, BEFORE SGETN: " << response.size() << endl;

    if (buf_size < content_length) {
      buf_size = content_length;
      if (buf) {
        free(buf);
      }
      buf = (char*)malloc(buf_size);
    }

    response.sgetn(buf, content_length);

    //cout << "RESPONSE SIZE, BEFORE JPEG CONSUME: " << response.size() << endl;

    //response.consume(content_length);

    //cout << "RESPONSE SIZE, AFTER JPEG CONSUME: " << response.size() << endl;

    //char buf2[10000] = {0};
    //response.sgetn(buf2, 1000);
    //ofstream o("out.bin", ios::binary);
    //o.write(buf2, 1000);

    Image image(decompress_jpeg(buf, content_length));

    if (!filter.is_initialized()) {
      filter.init(image.w, image.h, image.bytes_per_pixel);
    }

    filter.add(image);

    bool res(display_image.show(filter.get()));

    if (!res) {
      break;
    }
  }

  free(buf);

  return 0;
}

