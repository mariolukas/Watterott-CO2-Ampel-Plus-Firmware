const char javascript[] PROGMEM = R"=====(
var el = document.getElementById("ampel");
refreshAmpel();
setInterval(refreshAmpel, 10000);

function clearAmpel(){
  el.classList.remove("ampelgruen");
  el.classList.remove("ampelgelb");
  el.classList.remove("ampelrot");
  el.classList.remove("ampelrotblinkend");  
}

function refreshAmpel() {
    var xmlhttp = new XMLHttpRequest();
    var url = "sensors.json";
    xmlhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var state = JSON.parse(this.responseText);
            clearAmpel();
            document.getElementById("state").innerHTML = "<b>Co2:</b> "+state.co2+" ppm<br><b>Temperature:</b> "+state.temp + " &ordm;C<br><b>Illumination:</b> "+state.lux+" LUX <br><b>Humidity:</b> "+state.hum+" %<br><b>MQTT connected:</b> "+!!state.mqtt+"<br><br>Firmware: "+state.firmware;
            if (state.co2 < 800) {
               el.classList.add("ampelgruen");
            } else if (state.co2 < 1000) {
               el.classList.add("ampelgelb");
            } else if (state.co2 < 1200) {
               el.classList.add("ampelrot");
            } else {
              el.classList.add("ampelrotblinkend");
            }
        }
    };
    xmlhttp.open("GET", url, true);
    xmlhttp.send();
}
)=====";
