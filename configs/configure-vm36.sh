ip -6 route del fc00:1234:3::/64 via fc00:1234:2::26 dev eth1
ip -6 route add fc00:1234:3::/64 via fc00:1234:4::3 dev eth2
ip -6 route
