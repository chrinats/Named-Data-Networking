#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/point-to-point-net-device.h"

namespace ns3 {
//Cache Time of each entry in  Content Store
void
CacheEntryRemoved(std::string context, Ptr<const ndn::cs::Entry> entry, Time
lifetime)
{
std::cout << entry->GetName() << " " << lifetime.ToDouble(Time::S) << "s" <<
std::endl;
}

Int main(int argc, char* argv[])
{
//Buffer Size
Config::SetDefault ("ns3::QueueBase::MaxSize", StringValue ("200p"));
int consumernodes = 2; // Number of Consumers
int producernodes = 2; // Number of Producers
int routers = 2 ; // Number of Routers

//Initialization of values 
std::string cachingStrategy = "LRU";
std::string numCacheEntries = "1000";
std::string packetsize = "5000";
std::string interests = "900";
std::string delayvalue = "10ms";

// Insert values from keyboard
CommandLine cmd;
cmd.AddValue ("csStrategy", "cachingStrategy(LRU,LFU,FIFO,RANDOM,nocache)",
cachingStrategy);
cmd.AddValue ("cacheEntries", "numCacheEntries", numCacheEntries);
cmd.AddValue ("packetsize", "packetsize", packetsize);
cmd.AddValue ("interests", "interests pers second", interests);
cmd.AddValue ("delayvalue", "channel in [ms]", delayvalue);
cmd.Parse(argc, argv);

//Node containers creation
NodeContainer consumer;
NodeContainer router;
NodeContainer producer;

//Node Creation
consumer.Create(consumernodes);
producer.Create(producernodes);
router.Create(routers);

//Names added
Names::Add ("Consumer1", consumer.Get (0));
Names::Add ("Consumer2", consumer.Get (1));
Names::Add ("Rtr1", router.Get(0));
Names::Add ("Rtr2", router.Get(1));
Names::Add ("Producer1", producer.Get(0));
Names::Add ("Producer2", producer.Get(1));

//Point to Point connection
καταναλωτών- παραγωγών και δρομολογητή, όπως και μεταξύ των δύο δρομολογητών
PointToPointHelper p2p;
p2p.SetChannelAttribute ("Delay", StringValue (delayvalue));
p2p.SetDeviceAttribute ("DataRate", StringValue ("40Mbps"));
/*p2p.SetQueue("ns3::DropTailQueue","MaxPackets",UintegerValue(50)); */
PointToPointHelper rp2p;
rp2p.SetChannelAttribute ("Delay", StringValue (delayvalue));
rp2p.SetDeviceAttribute ("DataRate", StringValue ("60Mbps"));
//rp2p.SetQueue("ns3::DropTailQueue","MaxPackets",UintegerValue(80));

//Connection installation
p2p.Install(consumer.Get(0), router.Get(0));
p2p.Install(consumer.Get(1), router.Get(0));
rp2p.Install(router.Get(0), router.Get(1));
p2p.Install(router.Get(1), producer.Get(0));
p2p.Install(router.Get(1), producer.Get(1));

// Creation of NDN Stack
ndn::StackHelper ndnHelper;

// Old Content Store utilization
if (cachingStrategy == "nocache"){
std::cout << "Setting cache of routers to no-cache" << std::endl;
ndnHelper.SetOldContentStore("");
ndnHelper.setCsSize(1); // disable content store
}
else if (cachingStrategy == "LRU") {
std::cout << "Using LRU caching strategies on routers " << std::endl;
ndnHelper.setCsSize(0); // use old content store
ndnHelper.SetOldContentStore ("ns3::ndn::cs::Stats::Lru","MaxSize",
numCacheEntries);
ndnHelper.Install(consumer);
ndnHelper.Install(producer);
ndnHelper.Install(router);

//Cache entry Lifetime
Config::Connect("/NodeList/5/$ns3::ndn::cs::Stats::Lru/WillRemoveEntry",
MakeCallback(CacheEntryRemoved));
}
else if (cachingStrategy == "LFU"){
std::cout << "Using LFU caching strategies on routers " << std::endl;
ndnHelper.setCsSize(0); // use old content store
ndnHelper.SetOldContentStore ("ns3::ndn::cs::Stats::Lfu","MaxSize",
numCacheEntries);
ndnHelper.Install(consumer);
ndnHelper.Install(producer);
ndnHelper.Install(router);

//Cache entry Lifetime
Config::Connect("/NodeList/5/$ns3::ndn::cs::Stats::Lfu/WillRemoveEntry",
MakeCallback(CacheEntryRemoved));
}
else if (cachingStrategy == "FIFO") {
std::cout << "Using FIFO caching strategies on routers " << std::endl;
ndnHelper.setCsSize(0); // use old content store
ndnHelper.SetOldContentStore ("ns3::ndn::cs::Stats::Fifo","MaxSize",
numCacheEntries);
ndnHelper.Install(consumer);
ndnHelper.Install(producer);
ndnHelper.Install(router);

//Cache entry Lifetime
Config::Connect("/NodeList/5/$ns3::ndn::cs::Stats::Fifo/WillRemoveEntry",
MakeCallback(CacheEntryRemoved));
}
else if (cachingStrategy == "RANDOM") {
std::cout << "Using RANDOM caching strategies on routers " << std::endl;
ndnHelper.setCsSize(0); // use old content store
ndnHelper.SetOldContentStore ("ns3::ndn::cs::Stats::Random","MaxSize",
numCacheEntries);
ndnHelper.Install(consumer);
ndnHelper.Install(producer);
ndnHelper.Install(router);

//Cache entry Lifetime
Config::Connect("/NodeList/5/$ns3::ndn::cs::Stats::Random/WillRemoveEntry",
MakeCallback(CacheEntryRemoved));
}
else {
fprintf(stderr, "No proper caching strategy selected!");
return 1;
}

//Choosing forward strategy
ndn::StrategyChoiceHelper::InstallAll("/prefix",
"/localhost/nfd/strategy/best-route");

// Global routing installation
ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
ndnGlobalRoutingHelper.InstallAll();

//Consumer application
ndn::AppHelper::consumerHelper("ns3::ndn::ConsumerZipfMandelbrot");
consumerHelper.SetAttribute("Frequency", StringValue(interests));
consumerHelper.SetAttribute("NumberOfContents", StringValue("2000"));//
Ορισμός αριθμού διαφορετικού περιεχομένου για την ZipfMandelbrot

// Consumer prefix
consumerHelper.SetPrefix("/dst1");
consumerHelper.Install(consumer.Get(0));
consumerHelper.SetPrefix("/dst2");
consumerHelper.Install(consumer.Get(1));

// Producer application
ndn::AppHelper producerHelper("ns3::ndn::Producer");
producerHelper.SetAttribute("PayloadSize", StringValue(packetsize));
ndnGlobalRoutingHelper.AddOrigins("/dst1", producer.Get(0));

// Producer prefix
producerHelper.SetPrefix("/dst1");
producerHelper.Install(producer.Get(0));
ndnGlobalRoutingHelper.AddOrigins("/dst2", producer.Get(1));
producerHelper.SetPrefix("/dst2");
producerHelper.Install(producer.Get(1));

// FIBs calculation
ndn::GlobalRoutingHelper::CalculateRoutes();
std::cout << "Starting simulation :)" << std::endl;
Simulator::Stop(Seconds(30.0));

//Call of L3RateTracer, CsTracer, AppDelayTracer
ndn::L3RateTracer::InstallAll("through.txt", Seconds(0.5));
ndn::CsTracer::InstallAll( "cs.txt", Seconds(0.5));
ndn::AppDelayTracer::InstallAll("delay.txt");
Simulator::Run();
Simulator::Destroy();
std::cout << "Simulation ended" << std::endl;
return 0;
}
}
// namespace ns3
int
main(int argc, char* argv[])
{
return ns3::main(argc, argv);
}