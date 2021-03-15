#include <gnc_functions.hpp>
//include API 

int main(int argc, char** argv)
{
	//initialize ros 
	//argc and char** argv allows us to enter command line arguments to ROS 
	ros::init(argc, argv, "gnc_node");
	ros::NodeHandle gnc_node("~");
	
	//initialize control publisher/subscribers
	// publishers send data out from the node
	// subscribers bring/accept data into the node
	init_publisher_subscriber(gnc_node);

  	// wait for FCU connection
  	// wait until connection from computer/(GCS?) is received then move on
	wait4connect();

	//wait for user to switch to mode GUIDED
	wait4start();

	//create local reference frame 
	initialize_local_frame();

	//request takeoff
	takeoff(3);

	//specify some waypoints 
	std::vector<gnc_api_waypoint> waypointList;// vector is of of type <gnc_api_waypoint> with name 'waypointlist'
	                                           // vector is C++ container is like an array but can dynamically size the vector
	gnc_api_waypoint nextWayPoint; //specify a data container 'nextWayPoint' of type <gnc_api_waypoint> that can be pushed into the vector list
	                               // gnc_api_waypoint is a data container with attributes .x, .y .z, and .psi
	nextWayPoint.x = 0;
	nextWayPoint.y = 0;
	nextWayPoint.z = 3;
	nextWayPoint.psi = 0;
	waypointList.push_back(nextWayPoint); //add the previous waypoints to waypointlist with pushback command
	nextWayPoint.x = 5;
	nextWayPoint.y = 0;
	nextWayPoint.z = 3;
	nextWayPoint.psi = -90;
	waypointList.push_back(nextWayPoint);
	nextWayPoint.x = 5;
	nextWayPoint.y = 5;
	nextWayPoint.z = 3;
	nextWayPoint.psi = 0;
	waypointList.push_back(nextWayPoint);
	nextWayPoint.x = 0;
	nextWayPoint.y = 5;
	nextWayPoint.z = 3;
	nextWayPoint.psi = 90;
	waypointList.push_back(nextWayPoint);
	nextWayPoint.x = 0;
	nextWayPoint.y = 0;
	nextWayPoint.z = 3;
	nextWayPoint.psi = 180;
	waypointList.push_back(nextWayPoint);
	nextWayPoint.x = 0;
	nextWayPoint.y = 0;
	nextWayPoint.z = 3;
	nextWayPoint.psi = 0;
	waypointList.push_back(nextWayPoint);


	//specify control loop rate. 
	// We recommend a low frequency to not over load the FCU with messages. Too many messages will cause the drone to be sluggish
	ros::Rate rate(2.0); //rate counter, how quickly for the loop to run, in this case it is 2.0 Hz
	int counter = 0; //tracks which waypoint we are out
	while(ros::ok()) //ros::ok() is a ros function, if it is not 'ok' then the while loop will break
	{
		ros::spinOnce(); //ros::spinOnce() is a ROS function. Allows all the publishers to push data and subscribers to pull data, so its like an update command
		rate.sleep(); // this will enforce the 2 Hz data rate
		if(check_waypoint_reached(.3) == 1) // check_waypoint_reached() is a GNC API function which just checks to see if the drone is at the coordinates
		                                   // We do this to ensure the drone has taken off (since the takeoff coords is a waypoint)
		                                   // the .3 is a tolerance of how close the drone needs to be to that position, units are in meters
		{
			if (counter < waypointList.size()) //this will iteratively go through each waypoint by incrementing the counter
			                                   // Once the counter size is greater than the waypoint size, we have gone through all our coordinates
			{
				set_destination(waypointList[counter].x,waypointList[counter].y,waypointList[counter].z, waypointList[counter].psi);
				counter++;	
			}else{
				//land after all waypoints are reached
				land();
			}	
		}	
		
	}
	return 0;
}