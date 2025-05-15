#include <roverlib.h>
#include <sys/time.h>
#include <unistd.h>

long long current_time_millis() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long)(tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

// The main user space program
// this program has all you need from roverlib: service identity, reading,
// writing and configuration
int user_program(Service service, Service_configuration *configuration) {
  // Setting the buffer of stdout to NULL in order to print the logs in the
  // webUI
  setbuf(stdout, NULL);
  //
  // Get configuration values
  //
  if (configuration == NULL) {
    printf("Configuration cannot be accessed\n");
    return 1;
  }

  //
  // Access the service identity, who am I?
  //
  printf("Hello world, a new C service '%s' was born at version %s\n",
         service.name, service.version);

  //
  // Access the service configuration, to use runtime parameters
  //
  double *tunable_speed = get_float_value_safe(configuration, "speed");
  if (tunable_speed == NULL) {
    printf("Failed to get configuration\n");
    return 1;
  }
  printf("Fetched runtime configuration example tunable number: %f\n",
         *tunable_speed);

  //
  // Reading from an input, to get data from other services (see service.yaml to
  // understand the input name)
  //
  read_stream *read_stream = get_read_stream(&service, "imaging", "path");
  if (read_stream == NULL) {
    printf("Failed to get read stream\n");
  }

  //
  // Writing to an output that other services can read (see service.yaml to
  // understand the output name)
  //
  write_stream *write_stream = get_write_stream(&service, "decision");
  if (write_stream == NULL) {
    printf("Failed to create write stream 'decision'\n");
  }

  while (true) {
    // Read one message from the stream
    ProtobufMsgs__SensorOutput *data = read_pb(read_stream);
    if (data == NULL) {
      printf("Failed to read from 'imaging' service\n");
    }

    // When did imaging service create this message?
    int created_at = data->timestamp;
    printf("Received message with timestamp: %d\n", created_at);

    // Get the imaging data
    ProtobufMsgs__CameraSensorOutput *imaging_data = data->cameraoutput;
    if (imaging_data == NULL) {
      printf("Message does not contain camera output. What did imaging do??");
      return 1;
    }
    printf("Imaging service captured a %d by %d image\n",
           imaging_data->trajectory->width, imaging_data->trajectory->height);

    // First check if imaging detected any track edges, if it did
    // trajectory->points will not be NULL
    if (imaging_data->trajectory->points != NULL) {
      // Print the X and Y coordinates of the middle point of the track that
      // Imaging has detected
      printf("The X: %d and Y: %d values of the middle point of the track\n",
             imaging_data->trajectory->points[0]->x,
             imaging_data->trajectory->points[0]->y);
    }

    // This value holds the steering position that we want to pass to the servo
    // (-1 = left, 0 = center, 1 = right)
    float steer_position = -0.5;

    // Initialize the message that we want to send to the actuator
    ProtobufMsgs__SensorOutput actuator_msg =
        PROTOBUF_MSGS__SENSOR_OUTPUT__INIT;
    // Set the message fields
    actuator_msg.timestamp = current_time_millis(); // milliseconds since epoch
    actuator_msg.status = 0;                        // all is well
    actuator_msg.sensorid = 1;
    // Set the oneof field contents
    ProtobufMsgs__ControllerOutput controller_output =
        PROTOBUF_MSGS__CONTROLLER_OUTPUT__INIT;
    controller_output.steeringangle = steer_position;
    controller_output.leftthrottle = *tunable_speed;
    controller_output.rightthrottle = *tunable_speed;
    controller_output.fanspeed = 0;
    controller_output.frontlights = false;
    // Set the oneof field (union)
    actuator_msg.controlleroutput = &controller_output;
    actuator_msg.sensor_output_case =
        PROTOBUF_MSGS__SENSOR_OUTPUT__SENSOR_OUTPUT_CONTROLLER_OUTPUT;

    // Send the message to the actuator
    int res = write_pb(write_stream, &actuator_msg);
    if (res <= 0) {
      printf("Could not write to actuator\n");
      return 1;
    }

    //
    // Now do something else fun, see if our "example-string-tunable" is updated
    //
    double *curr = tunable_speed;

    printf("Checking for tunable number update\n");

    // We are not using the safe version here, because using locks is boring
    // (this is perfectly fine if you are constantly polling the value)
    // nb: this is not a blocking call, it will return the last known value
    double *new_val = get_float_value(configuration, "speed");
    if (new_val == NULL) {
      printf("Failed to get updated tunable number\n");
      return 1;
    }

    if (curr != new_val) {
      printf("Tunable number updated: %f -> %f\n", *curr, *new_val);
      curr = new_val;
    }
    tunable_speed = curr;
  }
}

// This is just a wrapper to run the user program
// it is not recommended to put any other logic here
int main() { return run(user_program); }