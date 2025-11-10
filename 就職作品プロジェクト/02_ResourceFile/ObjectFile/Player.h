#include "Square.h"
#include "Camera.h"

class Player : public Square
{
private:
    
public:
    Player(Camera* cam);
    ~Player() override = default;

    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    
};