/**
 * Add gobals here
 */
var seconds 	= null;
var otaTimerVar =  null;
var wifiConnectInterval = null;
let tempPrinting = false;

/**
 * Initialize functions here.
 */
$(document).ready(function(){
	getUpdateStatus();
	startNTCSensorInterval();
	activate_listener('red_val');
	activate_listener('green_val');
	activate_listener('blue_val');
	console.log("ready");
	$("#send_rgb").on("click", function(){
		send_rgb_values();
	});
	$("#toogle_led").on("click", function(){
		toogle_led();
	}); 
	$("#apagar_uart").on("click", function(){
		toggle_terminal();
	}); 

	/** 
	*Initialize temperature button
	*/
    $("#toggle_temperature").on("click", function() {
        const temperatureData = document.getElementById("temperature_data");
        const toggleButton = document.getElementById("toggle_temperature");

        if (temperatureData.style.display === "none") {
            getNTCSensorValues();
            temperatureData.style.display = "block";
            toggleButton.value = "Hide Temperature";
        } else {
            temperatureData.style.display = "none";
            toggleButton.value = "Show Temperature";
        }
    });

	/** 
	 * Initialize potentiometer button
	 */
	$("#toggle_potentiometer").on("click", function() {
		const potentiometerData = document.getElementById("potentiometer_data");
		const toggleButton = document.getElementById("toggle_potentiometer");

		if (potentiometerData.style.display === "none") {
			potentiometerData.style.display = "block";
			toggleButton.value = "Hide Potentiometer Value";
			
			// Inicia la actualización automática mientras el potenciómetro esté visible
			setInterval(getPotentiometerValues, 1000);  // Actualiza el valor cada 1 segundo
		} else {
			potentiometerData.style.display = "none";
			toggleButton.value = "Show Potentiometer Value";
		}
	});

});   

/**
 * Gets file name and size for display on the web page.
 */        
function getFileInfo() 
{
    var x = document.getElementById("selected_file");
    var file = x.files[0];

    document.getElementById("file_info").innerHTML = "<h4>File: " + file.name + "<br>" + "Size: " + file.size + " bytes</h4>";
}

/**
 * Handles the firmware update.
 */
function updateFirmware() 
{
    // Form Data
    var formData = new FormData();
    var fileSelect = document.getElementById("selected_file");
    
    if (fileSelect.files && fileSelect.files.length == 1) 
	{
        var file = fileSelect.files[0];
        formData.set("file", file, file.name);
        document.getElementById("ota_update_status").innerHTML = "Uploading " + file.name + ", Firmware Update in Progress...";

        // Http Request
        var request = new XMLHttpRequest();

        request.upload.addEventListener("progress", updateProgress);
        request.open('POST', "/OTAupdate");
        request.responseType = "blob";
        request.send(formData);
    } 
	else 
	{
        window.alert('Select A File First')
    }
}

/**
 * Progress on transfers from the server to the client (downloads).
 */
function updateProgress(oEvent) 
{
    if (oEvent.lengthComputable) 
	{
        getUpdateStatus();
    } 
	else 
	{
        window.alert('total size is unknown')
    }
}

/**
 * Posts the firmware udpate status.
 */
function getUpdateStatus() 
{
    var xhr = new XMLHttpRequest();
    var requestURL = "/OTAstatus";
    xhr.open('POST', requestURL, false);
    xhr.send('ota_update_status');

    if (xhr.readyState == 4 && xhr.status == 200) 
	{		
        var response = JSON.parse(xhr.responseText);
						
	 	document.getElementById("latest_firmware").innerHTML = response.compile_date + " - " + response.compile_time

		// If flashing was complete it will return a 1, else -1
		// A return of 0 is just for information on the Latest Firmware request
        if (response.ota_update_status == 1) 
		{
    		// Set the countdown timer time
            seconds = 10;
            // Start the countdown timer
            otaRebootTimer();
        } 
        else if (response.ota_update_status == -1)
		{
            document.getElementById("ota_update_status").innerHTML = "!!! Upload Error !!!";
        }
    }
}

/**
 * Displays the reboot countdown.
 */
function otaRebootTimer() 
{	
    document.getElementById("ota_update_status").innerHTML = "OTA Firmware Update Complete. This page will close shortly, Rebooting in: " + seconds;

    if (--seconds == 0) 
	{
        clearTimeout(otaTimerVar);
        window.location.reload();
    } 
	else 
	{
        otaTimerVar = setTimeout(otaRebootTimer, 1000);
    }
}

/*Modify*/

/**
 * Gets  temperature values for display on the web page.
 */
function getNTCSensorValues()
{
	$.ajax({
		url: '/ntcSensor.json',
		dataType: 'json',
		method: 'GET',
		cache: false,
		success: function(data) {
			$("#temperature_reading").text(data["temp"]);
		},
		error: function(xhr, status, error)
		{
			console.error("Error fetching temperature data:", error);
		} 
	});
}

/**
 * Sets the interval for getting the updated DHT22 sensor values.
 */
function startNTCSensorInterval()
{
	setInterval(getNTCSensorValues, 5000);    
}


/**
 * Función para obtener el valor del potenciómetro
 * 
 */
function getPotentiometerValues() {
    $.ajax({
        url: '/PotVoltage.json',
        dataType: 'json',
        method: 'GET',
        cache: false,
        success: function(data) {
            console.log("Potentiometer Data:", data);  // Verifica en la consola que el valor se recibe correctamente
            if (data && data.volt !== undefined) {
                // Actualiza el valor en el HTML
                $("#potentiometer_reading").text(data["volt"]);
            } else {
                console.error("Datos inválidos del potenciómetro");
            }
        },
        error: function(xhr, status, error) {
            console.error("Error al obtener datos del potenciómetro:", error);
        }
    });
}

/**
 * Sets the interval for getting the updated DHT22 sensor values.
 */
function startPotentiometerValues()
{
	setInterval(getPotentiometerValues, 1000);    
}


/**
 * Clears the connection status interval.
 */
function stopWifiConnectStatusInterval()
{
	if (wifiConnectInterval != null)
	{
		clearInterval(wifiConnectInterval);
		wifiConnectInterval = null;
	}
}

/**
 * Gets the WiFi connection status.
 */
function getWifiConnectStatus()
{
	var xhr = new XMLHttpRequest();
	var requestURL = "/wifiConnectStatus";
	xhr.open('POST', requestURL, false);
	xhr.send('wifi_connect_status');
	
	if (xhr.readyState == 4 && xhr.status == 200)
	{
		var response = JSON.parse(xhr.responseText);
		
		document.getElementById("wifi_connect_status").innerHTML = "Connecting...";
		
		if (response.wifi_connect_status == 2)
		{
			document.getElementById("wifi_connect_status").innerHTML = "<h4 class='rd'>Failed to Connect. Please check your AP credentials and compatibility</h4>";
			stopWifiConnectStatusInterval();
		}
		else if (response.wifi_connect_status == 3)
		{
			document.getElementById("wifi_connect_status").innerHTML = "<h4 class='gr'>Connection Success!</h4>";
			stopWifiConnectStatusInterval();
		}
	}
}

/**
 * Starts the interval for checking the connection status.
 */
function startWifiConnectStatusInterval()
{
	wifiConnectInterval = setInterval(getWifiConnectStatus, 2800);
}

/**
 * Send the RGB values.
 */
function send_rgb_values() {
    // Obtiene los valores de los campos de entrada
    const red_val = parseInt($("#red_val").val());
    const green_val = parseInt($("#green_val").val());
    const blue_val = parseInt($("#blue_val").val());
    const on_time = parseInt($("#on_time").val());
    const off_time = parseInt($("#off_time").val());

    // Crea el objeto JSON que se enviará
    const jsonData = {
        red_val: red_val,
        green_val: green_val,
        blue_val: blue_val,
        on_time: on_time,
        off_time: off_time,
        timestamp: Date.now()
    };

    // Realiza una solicitud AJAX al servidor
    $.ajax({
        url: '/rgb_vals.json',
        dataType: 'json',
        method: 'POST',
        contentType: 'application/json',  // Especifica que el contenido es JSON
        cache: false,
        data: JSON.stringify(jsonData),  // Convierte el objeto JSON a una cadena
        success: function(response) {
            console.log("RGB values sent successfully:", response);
        },
        error: function(xhr, status, error) {
            console.error("Error sending RGB values:", error);
        }
    });
}


function toggle_terminal() {
    const action = tempPrinting ? "temp_off" : "temp_on";  // Alternar entre "temp_on" y "temp_off"
    tempPrinting = !tempPrinting;  // Cambiar el estado

    $.ajax({
        url: '/toggle_uart.json',
        dataType: 'json',
        method: 'POST',
        cache: false,
        success: function(response) {
            console.log("UART action completed:", response);  // Opcional: para depuración en el navegador
        },
        error: function(xhr, status, error) {
            console.error("Error sending UART action:", error);  // Opcional: para depuración en el navegador
        }
    });
}


/**
 * toogle led function.
 */
function toogle_led()
{
	$.ajax({
		url: '/toogle_led.json',
		dataType: 'json',
		method: 'POST',
		cache: false,
		success: function(response) {
            console.log("LED toggled successfully:", response);
        },
        error: function(xhr, status, error) {
            console.error("Error toggling LED:", error);
        }
    });

}


function activate_listener( used_id ){
	
	const myInput = document.getElementById( used_id );

	myInput.addEventListener('input', () => {
	if (myInput.value > 255) {
		myInput.value = 255;
	}
	if (!Number.isInteger(Number(myInput.value))) {
		myInput.value = Math.floor(Number(myInput.value));
	}
	});
}