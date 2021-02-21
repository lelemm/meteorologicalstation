class RainDetector {
    protected:
        int rainPin;
    public:
        RainDetector(int _rainPin);
        bool isRainning();
};