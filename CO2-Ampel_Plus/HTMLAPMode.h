
/**
 * Confirm Page of AP
 */

const String ap_save_html =
  F("<html>\
<head>\
<title>WiFiNINA_WebServer POST handling</title>\
<style>\
#file-input, input {\
        width: 90%;\
        height: 44px;\
        border-radius: 4px;\
        margin: 10px auto;\
        font-size: 15px\
}\
input {\
    background: #f1f1f1;\
    border: 0;\
    padding: 0 15px\
}\
body {\
    background: #3498db;\
    font-family: sans-serif;\
    font-size: 14px;\
    color: #777\
}\
#file-input {\
    padding: 0;\
    border: 1px solid #ddd;\
    line-height: 44px;\
    text-align: left;\
    display: block;\
    cursor: pointer\
}\
#bar, #prgbar {\
    background-color: #f1f1f1;\
    border-radius: 10px\
}\
#bar {\
    background-color: #3498db;\
    width: 0%;\
    height: 10px\
}\
.box {\
    background: #fff;\
    max-width: 258px;\
    margin: 75px auto;\
    padding: 30px;\
    border-radius: 5px;\
    text-align: center\
}\
.btnbox {\
    background: #fff;\
    max-width: 258px;\
    border-radius: 5px;\
    text-align: center\
}\
.btn {\
    background: #3498db;\
    color: #fff;\
    cursor: pointer;\
    width: 90%;\
    height: 44px;\
    border-radius: 4px;\
    margin: 10px auto;\
    font-size: 15px;\
}\
</style>\
<meta charset=\"UTF-8\">\
<title>CO2 Ampel Wifi AP</title>\
<meta http-equiv=\"refresh\" content=\"3; URL=/\">\
</head>\
<body>\
<div class=box>WiFi Settings saved.<br> Device will reboot soon. </div>\
</body>\
</html>");

/**
 * Main Page of  Access Point
 */

const String ap_root_html_header =
  F("<html>\
<head>\
<title>WiFiNINA_WebServer POST handling</title>\
<style>\
#file-input, input {\
        width: 90%;\
        height: 44px;\
        border-radius: 4px;\
        margin: 10px auto;\
        font-size: 15px\
}\
input {\
    background: #f1f1f1;\
    border: 0;\
    padding: 0 15px\
}\
body {\
    background: #3498db;\
    font-family: sans-serif;\
    font-size: 14px;\
    color: #777\
}\
#file-input {\
    padding: 0;\
    border: 1px solid #ddd;\
    line-height: 44px;\
    text-align: left;\
    display: block;\
    cursor: pointer\
}\
#bar, #prgbar {\
    background-color: #f1f1f1;\
    border-radius: 10px\
}\
#bar {\
    background-color: #3498db;\
    width: 0%;\
    height: 10px\
}\
.box {\
    background: #fff;\
    max-width: 258px;\
    margin: 75px auto;\
    padding: 30px;\
    border-radius: 5px;\
    text-align: center\
}\
.btnbox {\
    background: #fff;\
    max-width: 258px;\
    border-radius: 5px;\
    text-align: center\
}\
.btn {\
    background: #3498db;\
    color: #fff;\
    cursor: pointer;\
    width: 90%;\
    height: 44px;\
    border-radius: 4px;\
    margin: 10px auto;\
    font-size: 15px;\
}\
</style>\
<meta charset=\"UTF-8\">\
<title>CO2 Ampel Wifi AP</title>\
</head>\
<body>");

const String ap_root_html_footer =
  F("</body>\
</html>");
