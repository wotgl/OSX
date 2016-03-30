#include "Sniffer.h"


void initSniffer(const std::string interface) {
	Tins::SnifferConfiguration config;
	config.set_filter("port 80");
	config.set_promisc_mode(true);
	config.set_snap_len(400);

// Construct a Sniffer object, using the configuration above.
// Sniffer sniffer("eth0", config);
    Tins::Sniffer sniffer(interface, 2000);


    sniffer.sniff_loop(callback);
}

bool callback(const Tins::PDU &pdu) {


	const Tins::RawPDU &raw = pdu.rfind_pdu<Tins::RawPDU>();
    // raw.payload() returns a std::vector<uint8_t>
    // Tins::process_payload(raw.payload());
    std::vector<uint8_t> path = raw.payload();


    std::cout << "Handle Packet with size: " << path.size() << std::endl;
    // for (std::vector<uint8_t>::const_iterator i = path.begin(); i != path.end(); ++i)
    	// std::cout << *i;

    // std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;


    // const Tins::IP &ip = pdu.rfind_pdu<Tins::IP>();
    // const Tins::TCP &tcp = pdu.rfind_pdu<Tins::TCP>();
    // std::cout << ip.src_addr() << ':' << tcp.sport() << " -> "
    // << ip.dst_addr() << ':' << tcp.dport() << std::endl;
    return true;
}