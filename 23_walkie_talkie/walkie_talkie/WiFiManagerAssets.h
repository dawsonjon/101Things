#pragma once

/**
 * @brief HTML assets for captive portal used by WiFiManager classes.
 */

const char hero_img[] PROGMEM = R"=====(
    data:image/png;base64,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"
)=====";

const char lock_img[] PROGMEM = R"=====(
    data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAAAXNSR0IArs4c6QAAATZJREFUSEvdlb8uBHEUhb+jl0g0Eh0KrMcQQiIa4UUkGn8rFB6EaIUQj7GLQieikGj0R36bKTazM3N3Z1bBLed37/lyz725I345NIi+7Q3gGJjN8l+BQ0k3UX0IsL0HnJUI7Uq6qIJUAmyvAnfAB5BA95nYCnAOTAHLkh7LIBHgIQkAO5KuekVsbwOXwK2k9bqAL2ACGJf0nQNMAp/Au6TpugCnQkmFndqufO/WVg0oEojeCwG2N7OtmY9WMPf+lK3ude/3vg5svwGlngbQtqSlCND1tW7k51XUwf8FpOEdAanDdB7WimxsYtGCpOckansGSAevL5oAWpI6GWARaI8akCw6AMaAE2Br1ICBNreJRX8HkIaXhlgnOpJa0alIwzsF5oYkvAD7kqqP3ZCiYXr40w8VgoQf8KKEGdyIA9gAAAAASUVORK5CYII=
)=====";

const char root_template[] PROGMEM = R"=====(
<html>
<head>
    <meta name='viewport' content='width=device-width, initial-scale=1, minimum-scale=1'/>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background-color: #22272D;
        }

        .title {
            color: white;
            font-size: 28px;
            text-align: center;
        }

        .desc{
            color: #8293A6;
            font-size: 18px;
            text-align: center;
        }

        table {
            margin-left:auto;margin-right:auto;
        }

        .button {
            background-color: #22272D;
            border: #60BDC5;
            box-sizing: border-box;
            border-style: solid;
            border-width: 1px;
            border-radius: 4px;
            color: #8293A6;
            padding: 10px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 4px 2px;
            transition-duration: 0.4s;
            cursor: pointer;
        }

        .button:hover {
            background-color: #60BDC5;
            color: white;
        }

        tr td:first-child {
           color: white;
           text-align: right;
           padding-right: 5px;
           font-size: 16px;
        }

        tr td {
           color: #8293A6;
           text-align: left;
           padding-left: 5px;
           font-size: 16px;
        }
      </style>
</head>
<body>
    <table>
        <tr>
            <td>
                <img src="${hero.img}" width="100px" alt="Hero logo"/>
            </td>
        </tr>
    </table>
    </div>
    <div class="title">PicoW</div>
    <p/>
    <div class="desc">Configure the WiFi connection for the following device</div>
    <p/>
    <table>
        <tr>
            <td>Name</td>
            <td>${firmware.name}</td>
        </tr>
        <tr>
            <td>Short name</td>
            <td>${firmware.shortname}</td>
        </tr>
        <tr>
            <td>Maker</td>
            <td>${firmware.maker}</td>
        </tr>
        <tr>
            <td>Version</td>
            <td>${firmware.version}</td>
        </tr>
    </table>
    <p/>
    <table>
        <tr>
            <td>
                <a class="button" href="/wifi">Configure WiFi</a>
            </td>
        </tr>
    </table>
</body>
</html>
)=====";

const char wifi_template[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta name='viewport' content='width=device-width, initial-scale=1, minimum-scale=1'/>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background-color: #22272D;
        }

        .title {
            color: white;
            font-size: 28px;
            text-align: center;
        }

        .desc{
            color: #8293A6;
            font-size: 18px;
            text-align: center;
        }

        table {
            margin-left:auto;margin-right:auto;
        }

        .button {
            background-color: #22272D;
            border: #60BDC5;
            border-style: solid;
            border-width: 1px;
            border-radius: 4px;
            box-sizing: border-box;
            color: #8293A6;
            cursor: pointer;
            display: inline-block;
            padding: 10px 32px;
            font-size: 16px;
            margin: 4px 2px;
            text-align: center;
            text-decoration: none;
            transition-duration: 0.4s;
        }

        .button:hover {
            background-color: #60BDC5;
            color: white;
        }

        tr td {
           color: white;
           padding: 5px;
           font-size: 16px;
        }

        a {
            color: #48c0c7
        }

        .sec {
            text-align: right;
            width: 80px;
        }

        /* From https://hungyi.net/posts/pure-css-signal-indicator/ */
        .signal-icon {
            height: 18px;
            width: 18px;
            /* To show you the power of flexbox! */
            display: flex;
            /* Bars should be placed left to right */
            flex-direction: row;
            /* Evenly space the bars with space in between */
            justify-content: space-between;
            /* Sink the bars to the bottom, so they go up in steps */
            align-items: baseline;
        }

        .signal-icon .signal-bar {
            /* 4 + 3 + 4 + 3 + 4 = 18px (as set above)
                4px per bar and 3px margins between */
            width: 4px;
            /* All bars faded by default */
            opacity: 30%;
            /* Choose a color */
            background: white;
         }

        /* 3 different heights for 3 different bars */
        .signal-icon .signal-bar:nth-child(1) { height: 40%; }
        .signal-icon .signal-bar:nth-child(2) { height: 70%; }
        .signal-icon .signal-bar:nth-child(3) { height: 100%; }

        /* Emphasize different bars depending on
        weak/medium/strong classes */
        .signal-icon.weak .signal-bar:nth-child(1),
        .signal-icon.medium .signal-bar:nth-child(1),
        .signal-icon.medium .signal-bar:nth-child(2),
        .signal-icon.strong .signal-bar:nth-child(1),
        .signal-icon.strong .signal-bar:nth-child(2),
        .signal-icon.strong .signal-bar:nth-child(3)
        { opacity: 100%; }

        input[type=text], input[type=password] {
            padding: 6px;
            margin:4px 0;
            width:100%;
        }

        form {
            text-align: center;
        }

        td.sec img#lock {
            content: url(${img.lock});
            width: 20px;
        }

        td.sec img#unlock {
            /*content: url(${img.unlock});*/ /* no icon shown for insecure WiFi */
            width: 20px;
        }
    </style>
    <script>
        function setSSID(e) {
            var ssid = e.innerHTML;
            document.getElementById('ssid').value = ssid;
        }
    </script>
</head>
<body>
    <table>
        <tr>
            <td>
                <img src="${hero.img}" width="100px" alt="Hero logo"/>
            </td>
        </tr>
    </table>
    </div>
    <div class="title">PicoW</div>
    <p/>
    <div class="desc">Available WiFi Networks</div>
    <p/>
    <table>
        ${networks}
    </table>
    <p/>
    <br/>
    <table>
        <tr>
            <td>
            <div class="desc">WiFi Settings</div>
            </td>
        </tr>
    </table>
    <table>
        <tr>
            <td width="200px">
                <form method='POST' action='wifisave'>
                    <input id="ssid" type='text' placeholder='SSID' maxlength='32' name='n'/><br />
                    <input type='password' placeholder='Password' maxlength='32' name='p'/><br />
                    <input class="button" type='submit' value='Connect'/>
                </form>
            </td>
        </tr>
    </table>
    <table>
        <tr>
            <td>
                <p><a href='/'>Back to home</a></p>
            </td>
        </tr>
    </table>
</body>
</html>
)=====";

const char wifi_network_template[] PROGMEM = R"=====(
    <tr>
        <td>
            <div class="signal-icon ${signal}">
                <div class="signal-bar"></div>
                <div class="signal-bar"></div>
                <div class="signal-bar"></div>
            </div>
        </td>
        <td><a onclick="javascript:setSSID(this)" href="javascript:">${name}</a></td>
        <td class="sec">
            <img id="${network.security}"/>
        </td>
    </tr>
)=====";
