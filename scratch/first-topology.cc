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

void TearDownLink (Ptr<Node> nodeA, Ptr<Node> nodeB)
{
  nodeA->GetObject<Ipv4> ()->SetDown (1);
  nodeB->GetObject<Ipv4> ()->SetDown (1);
  nodeB->GetObject<Ipv4> ()->SetDown (2);

}

void UpLink (Ptr<Node> nodeA, Ptr<Node> nodeB)
{
  nodeA->GetObject<Ipv4> ()->SetUp (1);
  nodeB->GetObject<Ipv4> ()->SetUp (1);
  nodeB->GetObject<Ipv4> ()->SetUp (2);
}

int main (int argc, char** argv)
{
  NodeContainer allNodes;
  allNodes.Create(5);

  NodeContainer n0n1 = NodeContainer (allNodes.Get(0), allNodes.Get(1));
  NodeContainer n1n2 = NodeContainer (allNodes.Get(1), allNodes.Get(2));
  NodeContainer n2n3 = NodeContainer (allNodes.Get(2), allNodes.Get(3));
  NodeContainer n3n4 = NodeContainer (allNodes.Get(3), allNodes.Get(4));



  // This add IP/TCP/UDP functionality to nodes
  InternetStackHelper internetStack;
  internetStack.Install (allNodes);
 
  // This creates a point to point network link
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("1ms"));
  NetDeviceContainer d0d1 = p2p.Install (n0n1);
  NetDeviceContainer d1d2 = p2p.Install (n1n2);
  NetDeviceContainer d2d3 = p2p.Install (n2n3);
  NetDeviceContainer d3d4 = p2p.Install (n3n4);


  // Set IPs to our devices
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i1 = ipv4.Assign (d0d1);
 
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i2 = ipv4.Assign (d1d2);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i3 = ipv4.Assign (d2d3);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i4 = ipv4.Assign (d3d4);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  UdpEchoServerHelper echoServer (9);
  ApplicationContainer server = echoServer.Install (allNodes.Get (4));
  server.Start (Seconds (1.0));
  server.Stop (Seconds (20.0));
 
  // Client that will send the packets
  UdpEchoClientHelper client (i3i4.GetAddress (1), 9);
  client.SetAttribute ("MaxPackets", UintegerValue (2));
  client.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  client.SetAttribute ("PacketSize", UintegerValue (1024));

  // Send packets
  ApplicationContainer apps = client.Install (allNodes.Get (0));
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (20.0));
 
  AnimationInterface anim("disgraca.xml");
  anim.SetConstantPosition (allNodes.Get(0), 10, 50);
  anim.SetConstantPosition (allNodes.Get(1), 30, 50);
  anim.SetConstantPosition (allNodes.Get(2), 50, 50);
  anim.SetConstantPosition (allNodes.Get(3), 70, 50);
  anim.SetConstantPosition (allNodes.Get(4), 90, 50);

  Ptr<Node> a = allNodes.Get(0);
  Ptr<Node> b = allNodes.Get(1);

  Simulator::Schedule (Seconds (2.01), &TearDownLink, a, b);
  Simulator::Schedule (Seconds (3.00), &UpLink, a, b);

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}