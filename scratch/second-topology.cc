#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
 
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
 
using namespace ns3;
 
NS_LOG_COMPONENT_DEFINE ("SimpleRoutingPing6Example");
 
int main (int argc, char** argv)
{
  NodeContainer allNodes;
  allNodes.Create(6);

  NodeContainer n0n1 = NodeContainer (allNodes.Get(0), allNodes.Get(1));
  NodeContainer n0n2 = NodeContainer (allNodes.Get(0), allNodes.Get(2));
  NodeContainer n1n3 = NodeContainer (allNodes.Get(1), allNodes.Get(3));
  NodeContainer n1n4 = NodeContainer (allNodes.Get(1), allNodes.Get(4));
  NodeContainer n2n3 = NodeContainer (allNodes.Get(2), allNodes.Get(3));
  NodeContainer n2n4 = NodeContainer (allNodes.Get(2), allNodes.Get(4));
  NodeContainer n3n5 = NodeContainer (allNodes.Get(3), allNodes.Get(5));
  NodeContainer n4n5 = NodeContainer (allNodes.Get(4), allNodes.Get(5));

  // This add IP/TCP/UDP functionality to nodes
  InternetStackHelper internetStack;
  internetStack.Install (allNodes);
 
  // This creates a point to point network link
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("1ms"));
  NetDeviceContainer d0d1 = p2p.Install (n0n1);
  NetDeviceContainer d0d2 = p2p.Install (n0n2);
  NetDeviceContainer d1d3 = p2p.Install (n1n3);
  NetDeviceContainer d2d4 = p2p.Install (n2n4);
  NetDeviceContainer d3d5 = p2p.Install (n3n5);
  NetDeviceContainer d4d5 = p2p.Install (n4n5);

  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d1d4 = p2p.Install (n1n4);
  NetDeviceContainer d2d3 = p2p.Install (n2n3);
  
  // Set IPs to our devices
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i1 = ipv4.Assign (d0d1);
  Ipv4InterfaceContainer i0i2 = ipv4.Assign (d0d2);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i3 = ipv4.Assign (d1d3);
  
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i4 = ipv4.Assign (d1d4);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i3 = ipv4.Assign (d2d3);
  
  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i4 = ipv4.Assign (d2d4);

  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i5 = ipv4.Assign (d3d5);
  Ipv4InterfaceContainer i4i5 = ipv4.Assign (d4d5);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  UdpEchoServerHelper echoServer (9);
  ApplicationContainer server = echoServer.Install (allNodes.Get (5));
  server.Start (Seconds (1.0));
  server.Stop (Seconds (20.0));
 
  // Client that will send the packets
  UdpEchoClientHelper client (i4i5.GetAddress (1), 9);
  client.SetAttribute ("MaxPackets", UintegerValue (2));
  client.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  client.SetAttribute ("PacketSize", UintegerValue (1024));

  // Send packets
  ApplicationContainer apps = client.Install (allNodes.Get (0));
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (20.0));
 
  AnimationInterface anim("disgraca2.xml");
  anim.SetConstantPosition (allNodes.Get(0), 10, 50);
  anim.SetConstantPosition (allNodes.Get(1), 30, 35);
  anim.SetConstantPosition (allNodes.Get(2), 30, 65);
  anim.SetConstantPosition (allNodes.Get(3), 50, 35);
  anim.SetConstantPosition (allNodes.Get(4), 50, 65);
  anim.SetConstantPosition (allNodes.Get(5), 70, 50);

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}