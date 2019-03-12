
namespace stamping
{

class Buttons
{
public:
    void init();
    void read();
    bool select() const;
    bool up() const;
    bool down() const;
    void log() const;
private:
    int upValue{0};
    int downValue{0};
    int selectValue{0};
    int backValue{0};
    unsigned long prevTime{0};
    static const unsigned long thresholdTime{500};
};
}
