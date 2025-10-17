
let scene, camera, rendered;

function parentWidth(elem) {
  return elem.parentElement.clientWidth;
}

function parentHeight(elem) {
  return elem.parentElement.clientHeight;
}

// Create events for the sensor readings
if (!!window.EventSource) {
  var source = new EventSource('/events');
  source.addEventListener('open', function(e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);

  source.addEventListener('1_readings', function(e) {
    //console.log("1_readings", e.data);
    var obj = JSON.parse(e.data);
    document.getElementById("pHValue").innerHTML = obj.pHValue;
  }, false);

  source.addEventListener('3_reading', function(e) {
    console.log("3_reading", e.data);
    document.getElementById("a").innerHTML = e.data;
  }, false);

  source.addEventListener('2_readings', function(e) {
    console.log("2_readings", e.data);
    var obj = JSON.parse(e.data);
    document.getElementById("dd").innerHTML = obj.dd;
  }, false);
}