ip -6 route del fc00:1234:4::/64 via fc00:1234:1::26 dev eth1
ip -6 route add fc00:1234:4::/64 via fc00:1234:3::1 dev eth2

