// Copyright Pololu Corporation.  For more information, see https://www.pololu.com/
stop_driving = true
block_set_drive = false
mouse_dragging = false
calibrated = false

function init() {
  poll()
  $("#joystick").bind("touchstart",touchmove)
  $("#joystick").bind("touchmove",touchmove)
  $("#joystick").bind("touchend",touchend)
  $("#joystick").bind("mousedown",mousedown)
  $(document).bind("mousemove",mousemove)
  $(document).bind("mouseup",mouseup)
}

function poll() {
  $.ajax({url: "status.json"}).done(update_status)
  if(stop_driving && !block_set_drive)
  {
    setDrive(0,0);
    stop_driving = false
  }
}

function update_status(json) {
  s = JSON.parse(json)
  $("#button0").html(s["buttons"][0] ? '1' : '0')
  $("#button1").html(s["buttons"][1] ? '1' : '0')
  $("#button2").html(s["buttons"][2] ? '1' : '0')

  $("#battery_millivolts").html(s["battery_millivolts"])

  $("#analog0").html(s["analog"][0])
  $("#analog1").html(s["analog"][1])
  $("#analog2").html(s["analog"][2])
  $("#analog3").html(s["analog"][3])
  $("#analog4").html(s["analog"][4])
  $("#analog5").html(s["analog"][5])
  
  $("#encoders0").html(s["encoders"][0])
  $("#encoders1").html(s["encoders"][1])

  if(!calibrated && s["calibrated"])
  {
    calibrateDone()
    calibrated = true
  }

  setTimeout(poll, 100)
}

function touchmove(e) {
  e.preventDefault()
  touch = e.originalEvent.touches[0] || e.originalEvent.changedTouches[0];
  dragTo(touch.pageX, touch.pageY)
}

function mousedown(e) {
  e.preventDefault()
  mouse_dragging = true
}

function mouseup(e) {
  if(mouse_dragging)
  {
    e.preventDefault()
    mouse_dragging = false
    stop_driving = true
  }
}

function mousemove(e) {
  if(mouse_dragging)
  {
    e.preventDefault()
    dragTo(e.pageX, e.pageY)
  }
}

function dragTo(x, y) {
  elm = $('#joystick').offset();
  x = x - elm.left;
  y = y - elm.top;
  w = $('#joystick').width()
  h = $('#joystick').height()

  x = (x-w/2.0)/(w/2.0)
  y = (y-h/2.0)/(h/2.0)

  if(x < -1) x = -1
  if(x > 1) x = 1
  if(y < -1) y = -1
  if(y > 1) y = 1

  left_speed = Math.round(-40*y+20*x)
  right_speed = Math.round(-40*y-20*x)

  if(left_speed > 40) left_speed = 40
  if(left_speed < -40) left_speed = -40

  if(right_speed > 40) right_speed = 40
  if(right_speed < -40) right_speed = -40

  stop_driving = false
  setDrive(left_speed, right_speed)
}

function touchend(e) {
  e.preventDefault()
  stop_driving = true
}

function setDrive(left, right) {
  $("#joystick").html("Drive: " + left + " "+ right)

  if(block_set_drive) return
  block_set_drive = true

  $.ajax({url: "drive/"+left+","+right}).done(setDriveDone)
}

function setDriveDone() {
  block_set_drive = false
}

function setLeds() {
  led0 = $('#led0')[0].checked ? 1 : 0
  led1 = $('#led1')[0].checked ? 1 : 0
  led2 = $('#led2')[0].checked ? 1 : 0
  $.ajax({url: "leds/"+led0+","+led1+","+led2})
}

function playNotes() {
  notes = $('#notes').val()
  $.ajax({url: "play_notes/"+notes})
}

function shutdown() {
  if (confirm("Really shut down the Raspberry Pi?"))
    return true
  return false
}

function calibrate() {
  $("#calibrate").prop("disabled", true)
  $("#calibrate").html("Calibrating...")
  $.ajax({url: "calibrate"}).done(calibrateDone)
}

function calibrateDone() {
  $("#joystick").show()
  $("#standup").show()
  $("#calibrate").prop("disabled", false)
  $("#calibrate").html("Recalibrate")
}

function standUp() {
  $.ajax({url: "stand_up"})
}
