struct BlueCoinInstance {
    unsigned char ID;
    bool Collected;
};

class BlueCoinHolder {
    public:
    BlueCoinInstance* Coins;
    unsigned char Count;
    BlueCoinHolder();
};