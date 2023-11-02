/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/fd-net-device-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/abort.h"
#include "ns3/internet-apps-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//
 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");


static void
PingRtt (std::string context, Time rtt)
{
  NS_LOG_UNCOND ("Received Response with RTT = " << rtt);
}



int
main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  
  GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));

  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));


  std::string deviceName1 ("enp6s0");
  std::string deviceName2 ("enx3c18a0992c61");


  std::string remote1 ("169.254.239.25"); // example.com
  std::string remote2 ("169.254.239.21"); // example.com


  Ipv4Address gateway1 ("169.254.239.23"); // LMAIp
  Ipv4Address gateway2 ("169.254.239.24"); // CNIp

  Ipv4Mask localMask ("255.255.0.0");

  Ipv4Address remoteIp1 (remote1.c_str ());
  Ipv4Address remoteIp2 (remote2.c_str ());

  Ipv4Address localIp1 ("169.254.239.11");
  Ipv4Address localIp2 ("169.254.239.12");



  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  LogComponentEnable("EmuFdNetDeviceHelper",LOG_LEVEL_INFO );
  
  NS_LOG_INFO("create nodes");
  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("500Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);


  NS_LOG_INFO ("Create Device1");
  EmuFdNetDeviceHelper emu1;
  emu1.SetDeviceName (deviceName1);
  NetDeviceContainer devices1 = emu1.Install (nodes.Get(0));
  //Ptr<NetDevice> device1 = devices1.Get (0);
  //device1->SetAttribute ("Address", Mac48AddressValue (Mac48Address::Allocate ()));

  NS_LOG_INFO ("Create Device2");
  EmuFdNetDeviceHelper emu2;
  emu2.SetDeviceName (deviceName2);
  NetDeviceContainer devices2 = emu2.Install (nodes.Get(1));
  //Ptr<NetDevice> device2 = devices2.Get (0);
  //device2->SetAttribute ("Address", Mac48AddressValue (Mac48Address::Allocate ()));

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("169.254.0.0", "255.255.0.0", "0.0.239.10");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);
  /*NS_LOG_INFO ("Create channels.");
  EmuFdNetDeviceHelper emu;
  emu.SetDeviceName (deviceName);
  emu.SetAttribute ("EncapsulationMode", StringValue (encapMode));

  NetDeviceContainer d;
  Ipv4AddressHelper ipv4;
  Ipv4InterfaceContainer i;
  ApplicationContainer apps;

  ipv4.SetBase ("169.254.0.0", "255.255.0.0", "0.0.239.10");
  d = emu.Install (n);
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.NewAddress ();  // burn the 10.1.1.1 address so that 10.1.1.2 is next
  i = ipv4.Assign (d);*/

  

  /***UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
  ***/

  NS_LOG_INFO ("Create V4Ping Appliation2");
  Ptr<V4Ping> app2 = CreateObject<V4Ping> ();
  app2->SetAttribute ("Remote", Ipv4AddressValue (remoteIp2));
  app2->SetAttribute ("Verbose", BooleanValue (true) );
  nodes.Get(1)->AddApplication (app2);
  app2->SetStartTime (Seconds (1.0));
  app2->SetStopTime (Seconds (10.0));

//Names::Add ("app1", app1);
  Names::Add ("app2", app2);

  //Config::Connect ("/Names/app1/Rtt", MakeCallback (&PingRtt));
  Config::Connect ("/Names/app2/Rtt", MakeCallback (&PingRtt));


  //emu1.EnablePcap ("emu1-ping", device1, true);
  //emu2.EnablePcap ("emu2-ping", device2, true);

  pointToPoint.EnablePcapAll("emu");

  NS_LOG_INFO ("Run Emulation.");
  Simulator::Stop (Seconds (11.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
