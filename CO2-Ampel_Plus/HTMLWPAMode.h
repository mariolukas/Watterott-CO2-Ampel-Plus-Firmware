const char cssampel[] PROGMEM = R"=====(
.css-ampel {
  display: inline-block;
  width: 30px;
  height: 90px;
  border-radius: 6px;
  position: relative;
  background-color: black;
  zoom: 1.7;
}

.css-ampel span,
.css-ampel:before,
.css-ampel:after {
    content: "";
    color: white;
    position: absolute;
    border-radius: 15px;
    width: 22px;
    height: 22px;
    left: 4px;
}    

.css-ampel:before {
    top: 6px;
    background-color: red;
    background-color: dimgrey;
}

.css-ampel:after {
    top: 34px;
    background-color: yellow;
    background-color: dimgrey;
}

.css-ampel span {
    top: 62px;
    background-color: green;
    background-color: dimgrey;
}    

.ampelrot:before {
    background-color: red;
    box-shadow: 0 0 20px red;
}

.ampelgelb:after {
    background-color: yellow;
    box-shadow: 0 0 20px yellow;
}

.ampelgruen span {
    background-color: limegreen;
    box-shadow: 0 0 20px limegreen;
}

.ampelrotblinkend:before {
    animation-name: rotblinkend;
    animation-timing-function: ease-in;
    animation-duration: 2s; 
    animation-iteration-count: infinite;    
} 
   
@keyframes rotblinkend {
    0% {  
        background-color: dimgrey;
    }
    30% {  
        background-color: dimgrey;
    }
    40% {  
        background-color: red;
        box-shadow: 0 0 20px red;
    }
    100% {  
        background-color: red;
        box-shadow: 0 0 20px red;
    }
}
)=====";
const String wpa_root_html_header =
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
    max-width: 500px;\
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
}");

const String wpa_root_html_middle =
 F("</style>\
<meta charset=\"UTF-8\">\
<title>CO2 Ampel Wifi AP</title>\
<meta http-equiv=\"refresh\" content=\"3; URL=/\">\
</head>\
<body>");

const String wpa_root_html_footer =
  F("</body>\
</html>");
