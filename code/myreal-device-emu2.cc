#include "ns3/abort.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/fd-net-device-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/point-to-point-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("PingEmulationExample");

static void
PingRtt (std::string context, Time rtt)
{
  NS_LOG_UNCOND ("Received Response with RTT = " << rtt);
}

int
main (int argc, char *argv[])
{
  NS_LOG_INFO ("Ping Emulation Example");

  bool verbose = true;

  std::string deviceName1 ("enp6s0");
  std::string deviceName2 ("enx3c18a0992c61");


  CommandLine cmd (__FILE__);
  cmd.AddValue ("deviceName1", "Device name1", deviceName1);
  // cmd.AddValue ("remote1", "Remote IP address 1 (dotted decimal only please)", remote1);

  cmd.AddValue ("deviceName2", "Device name2", deviceName2);
  // cmd.AddValue ("remote2", "Remote IP address 2 (dotted decimal only please)", remote2);

  cmd.Parse (argc, argv);


  std::string remote1 ("169.254.239.25"); // example.com
  std::string remote2 ("169.254.239.21"); // example.com


  Ipv4Address gateway1 ("169.254.239.23"); // LMAIp
  Ipv4Address gateway2 ("169.254.239.24"); // CNIp

  Ipv4Mask localMask ("255.255.0.0");

  Ipv4Address remoteIp1 (remote1.c_str ());
  Ipv4Address remoteIp2 (remote2.c_str ());

  Ipv4Address localIp1 ("169.254.239.10");
  Ipv4Address localIp2 ("169.254.239.11");

  if(verbose){
    LogComponentEnable("PingEmulationExample",LOG_LEVEL_INFO ); 
    LogComponentEnable("EmuFdNetDeviceHelper",LOG_LEVEL_INFO ); 
  }


  GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));


  NS_LOG_INFO ("Create Node1");
  Ptr<Node> node1 = CreateObject<Node> ();
  NS_LOG_INFO ("Create Node2");
  Ptr<Node> node2 = CreateObject<Node> ();

  NodeContainer nodes(node1, node2);

  NS_LOG_INFO ("Create Device1");
  EmuFdNetDeviceHelper emu1;
  emu1.SetDeviceName (deviceName1);
  NetDeviceContainer devices1 = emu1.Install (node1);
  Ptr<NetDevice> device1 = devices1.Get (0);
  device1->SetAttribute ("Address", Mac48AddressValue (Mac48Address::Allocate ()));

  NS_LOG_INFO ("Create Device2");
  EmuFdNetDeviceHelper emu2;
  emu2.SetDeviceName (deviceName2);
  NetDeviceContainer devices2 = emu2.Install (node2);
  Ptr<NetDevice> device2 = devices2.Get (0);
  device2->SetAttribute ("Address", Mac48AddressValue (Mac48Address::Allocate ()));


  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("1500ms"));
  NetDeviceContainer p2p_link = pointToPoint.Install(nodes);


  NS_LOG_INFO ("Add Internet Stack on Device 1 and Device 2");
  InternetStackHelper internetStackHelper;
  internetStackHelper.Install (nodes);

  Ipv4AddressHelper addresses_localip;
  addresses_localip.SetBase ("169.254.0.0", "255.255.0.0", "0.0.239.10"); // ns3 node
  Ipv4InterfaceContainer interfaces = addresses_localip.Assign (p2p_link);

  /*Ipv4AddressHelper addresses_localip1;
  addresses_localip1.SetBase ("192.168.100.4", "255.255.255.255"); // ns3 node
  Ipv4InterfaceContainer interfaces = addresses_localip1.Assign (p2p_link.Get(0));

  Ipv4AddressHelper addresses_localip2;
  addresses_localip2.SetBase ("192.168.200.4", "255.255.255.255"); //ns3 node
  interfaces = addresses_localip2.Assign (p2p_link.Get(1));*/

  NS_LOG_INFO ("Create IPv4 Interface on device 1");
  Ptr<Ipv4> ipv4_1 = node1->GetObject<Ipv4> ();
  uint32_t interface1 = ipv4_1->AddInterface (device1);
  Ipv4InterfaceAddress address1 = Ipv4InterfaceAddress (localIp1, localMask);
  ipv4_1->AddAddress (interface1, address1);
  ipv4_1->SetMetric (interface1, 1);
  ipv4_1->SetUp (interface1);

  NS_LOG_INFO ("Create IPv4 Interface on device 2");
  Ptr<Ipv4> ipv4_2 = node2->GetObject<Ipv4> ();
  uint32_t interface2 = ipv4_2->AddInterface (device2);
  Ipv4InterfaceAddress address2 = Ipv4InterfaceAddress (localIp2, localMask);
  ipv4_2->AddAddress (interface2, address2);
  ipv4_2->SetMetric (interface2, 1);
  ipv4_2->SetUp (interface2);

  Ipv4StaticRoutingHelper ipv4RoutingHelper1;
  Ptr<Ipv4StaticRouting> staticRouting1 = ipv4RoutingHelper1.GetStaticRouting (ipv4_1);
  //staticRouting1->SetDefaultRoute (gateway1, interface1);

  // creating a path between ns3 interface1 to client device
  staticRouting1->AddHostRouteTo (Ipv4Address ("169.254.239.25"), Ipv4Address ("169.254.239.25"), interface1, 1);
  // static routing between p2p installed interfaces on node 1 and server device
  staticRouting1->AddHostRouteTo (Ipv4Address ("169.254.239.21"), Ipv4Address ("169.254.239.11"), interfaces.Get(0).second, 1);


  Ipv4StaticRoutingHelper ipv4RoutingHelper2;
  Ptr<Ipv4StaticRouting> staticRouting2 = ipv4RoutingHelper2.GetStaticRouting (ipv4_2);
  //staticRouting2->SetDefaultRoute (gateway2, interface2);

  // creating a path between ns3 interface1 to client device
  staticRouting2->AddHostRouteTo (Ipv4Address ("169.254.239.21"), Ipv4Address ("169.254.239.21"), interface2, 1);
  // static routing between p2p installed interfaces on node 1 and server device
  staticRouting2->AddHostRouteTo (Ipv4Address ("169.254.239.25"), Ipv4Address ("169.254.239.10"), interfaces.Get(1).second, 1);

  NS_LOG_INFO ("Create V4Ping Appliation1");
  Ptr<V4Ping> app1 = CreateObject<V4Ping> ();
  app1->SetAttribute ("Remote", Ipv4AddressValue (remoteIp1));
  app1->SetAttribute ("Verbose", BooleanValue (true) );
  node1->AddApplication (app1);
  app1->SetStartTime (Seconds (1.0));
  app1->SetStopTime (Seconds (10.0));

  NS_LOG_INFO ("Create V4Ping Appliation2");
  Ptr<V4Ping> app2 = CreateObject<V4Ping> ();
  app2->SetAttribute ("Remote", Ipv4AddressValue (remoteIp2));
  app2->SetAttribute ("Verbose", BooleanValue (true) );
  node2->AddApplication (app2);
  app2->SetStartTime (Seconds (1.0));
  app2->SetStopTime (Seconds (10.0));


  Names::Add ("app1", app1);
  Names::Add ("app2", app2);

  Config::Connect ("/Names/app1/Rtt", MakeCallback (&PingRtt));
  Config::Connect ("/Names/app2/Rtt", MakeCallback (&PingRtt));


  emu1.EnablePcap ("emu1-ping", device1, true);
  emu2.EnablePcap ("emu2-ping", device2, true);

  pointToPoint.EnablePcapAll("emu");

  NS_LOG_INFO ("Run Emulation.");
  Simulator::Stop (Seconds (1100.0));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
