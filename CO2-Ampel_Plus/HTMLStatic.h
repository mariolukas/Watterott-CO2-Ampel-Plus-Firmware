
/**
 * Confirmation Page
 */

const char save_settings_html[] PROGMEM = R"=====(
<head>
<meta charset=\"UTF-8\">
<title>CO2 Ampel Wifi Settings saved</title>
<meta http-equiv=\"refresh\" content=\"3; URL=/\">
<link rel="stylesheet" href="styles.css">
</head>
<body>
<div class="box">WiFi Settings saved.<br> Device will reboot soon. </div>
</body>
</html>
)=====";


/**
 * Settings Page
 */

const char settings_header_html[] PROGMEM = R"=====(
<html>
<head>
<meta charset="UTF-8">
<title>CO2 Ampel Wifi Settings</title>
<link rel="stylesheet" href="styles.css">
</head>
<body>
<div class="box">
 <form class="box" action="/save" method="POST" name="loginForm">
 <h1>Settings</h1>
)=====";

const char settings_footer_html[] PROGMEM = R"=====(
</div>
</body>
</html>
)=====";


/**
 * LogIn Page for Settings
 */


const char login_html[] PROGMEM = R"=====(
<html>
<head>
<link rel="stylesheet" href="styles.css">
</head>
<body>
<div class="box">
<form action="/login" method="POST"><br>
Password:<input type="password" name="login_password" placeholder="password"><br>
<input type="submit" name="SUBMIT" value="Submit">
</form>
</div>
<br>
)=====";


/**
 * Main HTML Page
 */
const char root_html[] PROGMEM = R"=====(
<head>
<meta charset="UTF-8">
<title>CO2 Ampel Wifi AP</title>
<link rel="stylesheet" href="styles.css">
<link rel="stylesheet" href="ampel.css">
</head>
<div class="box"><h1>CO2 Ampel State</h1>
<span id="ampel" class="css-ampel %s"><span class="cssampelspan"></span></span>
<br>
<br>
<span id="state">Waiting for initial state...</span>
</div>
<body></body></html>
<script type="text/javascript" src="/scripts.js"></script>
)=====";
