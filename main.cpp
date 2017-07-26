//this program is a screensaver which features randomly generated landscapes


//SFML headers
#include <SFML/Graphics.hpp>

//for random number generation
#include "random.h"

//for debugging logs - OMIT LATER
#include <iostream>

#include <fstream>



//this functions converts an HSL value to a RGB value
float Hue_2_RGB(float v1, float v2, float vH)             //Function Hue_2_RGB
{
    if ( vH < 0 )
        vH += 1;

    if ( vH > 1 )
        vH -= 1;

    if ( ( 6 * vH ) < 1 )
        return ( v1 + ( v2 - v1 ) * 6 * vH );

    if ( ( 2 * vH ) < 1 )
        return ( v2 );

    if ( ( 3 * vH ) < 2 )
        return ( v1 + ( v2 - v1 ) * ( ( 2 / 3 ) - vH ) * 6 );

    return ( v1 );
}


int main()
{


    //initiate random generator
    setRandomSeed();

    //calculate fullscreen size
    sf::VideoMode windowSize = sf::VideoMode::getFullscreenModes()[0];

    //create window with that size
    sf::RenderWindow window(windowSize, "", sf::Style::Default);

    //calculate what size blocks will be - minBlocks is how many blocks are desired on the shortest side
    int minBlocks = 50;
    int blockSize = window.getSize().x >= window.getSize().y ? window.getSize().y/minBlocks : window.getSize().x/minBlocks;

    //determine how many blocks it takes to fill the screen

    int blocksWide = window.getSize().x/blockSize;
    int blocksHigh = window.getSize().y/blockSize;

    //if a block does not fit exactly, it leaves an ugly black space, so we add blocks till
    //they overflow the boundary, which is more pleasing visually.

    while (blockSize*blocksWide < windowSize.width)
        blocksWide++;

    while (blockSize*blocksHigh < windowSize.height)
        blocksHigh++;

    bob:

    int minHeight, maxHeight, blendLine, terrainVariation;

    std::fstream file;

//    file.open(std::ios::in, "config.txt");

    //file.read();

    /*if (file.bad())
    {
            file.clear();

            maxHeight = blocksHigh/2, minHeight = blocksHigh/10, lastHeight = minHeight, terrainVariation = maxHeight/20, blendLine = minHeight/1.5, desiredHeight
    }
    else
    while (true)
    {
        std::string string;

        std::readline(file, string);

        if (string.find("maxHeight"))
            ;

    }
*/





    //create array of squares blocksWide*blocksHigh
    sf::RectangleShape ** rectangles = new sf::RectangleShape * [blocksWide];

    for (int i = 0; i < blocksWide; i++)
        rectangles[i] = new sf::RectangleShape[blocksHigh];

    //initiate all rectangles (size, color, position)
    for (int x = 0; x < blocksWide; x++)
        for (int y = 0; y < blocksHigh; y++)
        {
            rectangles[x][y].setSize(sf::Vector2f(blockSize, blockSize));
            rectangles[x][y].setPosition(x*blockSize, windowSize.height - y*blockSize);
            rectangles[x][y].setFillColor(sf::Color::Transparent);
        }

    //generate landscape - we make each slope within a random range of another,
    //and ensure they are within a minimum and max range

    //this line is for DEBUG purposes, to show height line - OMIT LATER
    sf::RectangleShape line(sf::Vector2f(windowSize.width, blockSize)), line2(sf::Vector2f(windowSize.width, blockSize));
    line.setFillColor(sf::Color(255, 0, 0, 25));
    line2.setFillColor(sf::Color(0, 0, 255, 25));


    for (int x = 0, maxHeight = blocksHigh/2, minHeight = blocksHigh/10, lastHeight = minHeight, terrainVariation = maxHeight/20, blendLine = minHeight/1.5, desiredHeight; x < blocksWide; x++)
    {
        //setting line height position for DEBUG - OMIT LATER
        line.setPosition(0, windowSize.height - (minHeight*blockSize));
        line2.setPosition(0, windowSize.height - (maxHeight*blockSize));

        //for every column (up and down slice), it can be a random range of a few blocks from the last slope height
        desiredHeight = lastHeight + randomRange(-terrainVariation, terrainVariation) > blendLine ? lastHeight + randomRange(-terrainVariation, terrainVariation) : blendLine + terrainVariation;

        //check to ensure it is within boundaries
        if (desiredHeight > maxHeight)
            desiredHeight = maxHeight;

        if (desiredHeight < minHeight)
            desiredHeight = minHeight;

        for (int y = 0, minLight, maxLight, lightPerLayer, red, green, blue; y < blocksHigh; y++)
        {
            //we create thecolumn of blocks by doing the following:
            //determine the base color of the "mountain." We then calculate
            //the numbers of layers we have to gradient over, and
            //find out how much light we should add to each block added
            //due to how things are shaded, we calculate the light range
            //differently for the upper part of stack > blendLine,
            //and the the <= blendLine part. Normally, each individual
            //collumn of blocks gradients at its own rate depending on
            //how many blocks it has, but if this is true all the way down,
            //the colors contrast much and are ugly, so we add a blendLine
            //blocks higher than this work as normal, and blocks on or under
            //the line all share the same shade for that height,
            //to make sure it blends nicely. Because they share the same color,
            //and we want it to all be, somewhat, one large gradient,
            //we give the upper stack a larger portion of the light to cover,
            //and the bottom parts we give a lower portion
            //once we have done this, we calculate the new RGB from the
            //base RGB and luminosity, and color the block this color
            //we also add grass layers in their own loop once the top of
            //that column has finished generating. Within it, we create
            //the grass base color and calculate its light range,
            //amount of layers, light per level, and color it accordingly
            //once we finish that loop, this loop also finishes, and we go to the next column
            //besides the coloring calculations, we check if we have placed the last
            //mountain/dirt block, (i.e. it was created at desiredHeight).
            //in the case we did, we then create the grassin a loop within. Once
            //this loop is concluded, we set the lastHeight to the last mountain
            //or dirt block, then break

            //dirt base hue and saturation - luminosity is defined by the min- and maxLight
            //and height of the block

            float hue = 59;
            float saturation = 100;

            //set range for light values, where the bottommost layer has the minLight lighting

            minLight = 0;
            maxLight = 100;

            //calculate the light change per layer by case (upper blocks or layer base blocks)
            lightPerLayer = y <= blendLine ? ((maxLight - minLight) / 4)/blendLine : (((maxLight - minLight) / 4)*3)/(desiredHeight - blendLine);

            //calculate the luminosity based on height, light per layer, and
            //if it is above the blend line or <= to it

float            luminosity = y <= blendLine ? y*lightPerLayer + minLight: (y - blendLine)*lightPerLayer + ((maxLight - minLight) / 4) + minLight;
//std::cout << "y " << y << " maxHeight: " << maxHeight << " Light-per-layer: " << lightPerLayer*y << "  luminosity: " << luminosity << std::endl;

luminosity /= 100;

luminosity++;


            //calculate the new color to be used from HSL components (I don't know how it works)


//std::cout << "red: " << red << "  green: " << green << "  blue:" << blue << std::endl;

            //set block's color with new RGB value
            rectangles[x][y].setFillColor(sf::Color(61*luminosity, 44*luminosity, 0*luminosity));

            //check if the last block we wanted was created, in which case we break
            if (y == desiredHeight)
            {
                int grassHeight = blocksHigh/20;
                //create grass
                for (int i = 0, red = randomRange(1, 255), green = randomRange(1, 255), blue = randomRange(1, 255); i < grassHeight; i++)
                {
                    minLight = 50;
                    maxLight = 100;
                    lightPerLayer = (maxLight - minLight)/grassHeight;
                    luminosity = i*lightPerLayer + minLight;
                    luminosity /= 100;
                    luminosity++;
                    red = 13*luminosity;
                    green = 93*luminosity;
                    blue = 0*luminosity;

                    if (red > 255)
                        red = 255;

                    if (blue > 255)
                        blue = 255;

                    if (green > 255)
                        green = 255;

                    //rectangles[x][y + i].setFillColor(sf::Color(30, 159, 0));
                    rectangles[x][y + i].setFillColor(sf::Color(red, green, blue));

                }

                lastHeight = desiredHeight;
                break;
            }
        }



    }

    ///tree generation - EXPLAIN HOW OR WHY
    for (int x = 1, lastTreeX = 0, treeMargin = 10; x < blocksWide - 1; x++)
        for (int y = 0; y < blocksHigh - 1; y++)
        {
            if (rectangles[x][y].getFillColor() == sf::Color::Transparent)
            {


                                if (x > lastTreeX + treeMargin && x + 1 < blocksWide && y > 0 && rectangles[x][y - 1].getFillColor() != sf::Color::Transparent && rectangles[x - 1][y].getFillColor() == sf::Color::Transparent && rectangles[x + 1][y].getFillColor() == sf::Color::Transparent)
                {

                    for (int a = 0, treeHeight = randomRange(5, 25); a < treeHeight; a++)
                    {
                        if (y + a < blocksHigh)
                        {
                            int red, green, blue;
                            red = 68;
                            green = 45;
                            blue = 2;
                            int minLight = 0;
                            int maxLight = 100;

                            int lightPerLayer = (maxLight - minLight)/treeHeight;
                            float luminosity = (a*lightPerLayer) + minLight;
                            luminosity /= 100;
                            luminosity += 1;

                            red *= luminosity;
                            green *= luminosity;
                            blue *= luminosity;

                            if (red > 255)
                        red = 255;

                    if (blue > 255)
                        blue = 255;

                    if (green > 255)
                        green = 255;

                            rectangles[x][y + a].setFillColor(sf::Color(red, green, blue));
                        }

                        if (y + a + 3 < blocksHigh && a + 1 >= treeHeight)
                        {
                            int failCount = 0;
                            sf::Color randomColor(randomRange(1, 255), randomRange(1, 255), randomRange(1, 255));

                            for (int d = y + a;  d >= (y + a) - (treeMargin/3); d--)
                            {
                                if (d >= blocksHigh)
                                    break;
                            std::cout << blocksHigh << "  " << d << std::endl;
                                rectangles[x][d].setFillColor(sf::Color::Transparent);
                            }

                            for (int q = 0, randomNumber = randomRange(0, 10), redBase = randomRange(25, 100), greenBase = randomRange(25, 100), blueBase = randomRange(25, 100); q <  static_cast<int>(((treeMargin/3)*2 + 1)*((treeMargin/3)*2 + 1) - randomNumber); q++)
                            {

                                int o, p;
                                while (true){

                                     failCount++;
                                     if (failCount > 500)
                                        break;
                                //std::cout << "FAILING: " << failCount << std::endl;
                                o = randomRange(-(treeMargin/3), treeMargin/3);
                                p = randomRange(-(treeMargin/3), treeMargin/3);

                                if (x + o < 0 || x + o >= blocksWide)
                                    continue;

                                if (y + a + p < 0 || y + a + p >= blocksHigh)
                                    continue;









                                if (rectangles[x + o][y + a + p].getFillColor() == sf::Color::Transparent)
                                    break;
                                else
                                    continue;
                                }

                                if (x + o < 0 || x + o >= blocksWide)
                                    continue;

                                if (y + a + p < 0 || y + a + p >= blocksHigh)
                                    continue;



                                    int red, green, blue;

                            int minLight = 70;
                            int maxLight = 150;

                            int lightPerLayer = (maxLight - minLight)/((treeMargin/3)*2 + 1);
                            float luminosity = ((p + (treeMargin/3 + 1))*lightPerLayer) + minLight;
                            luminosity /= 100;
                            luminosity += 1;

                            red = redBase * luminosity;
                            green = greenBase * luminosity;
                            blue = blueBase * luminosity;

if (red > 255)
                        red = 255;

                    if (blue > 255)
                        blue = 255;

                    if (green > 255)
                        green = 255;

                            rectangles[x + o][y + a + p].setFillColor(sf::Color(red, green, blue));
                                    //rectangles[x + o][y + a + p].setFillColor(sf::Color(21, 90, 0));
                                    //rectangles[x + o][y + a + p].setFillColor(randomColor);



                            }
                        }

                    }

                    lastTreeX = x;

                    break;
                }
            }
        }

    //makes ze cassle

    int xCord = randomRange(0 + blocksWide/3, (blocksWide/3)*2);
    int randomLength = randomRange(0, blocksWide/3);

    for (int x = xCord; x < blocksWide; x++)
        for (int y = 0; y < blocksHigh/2; y++)
        {
            int red = 50, green = 50, blue = 50;
             int minLight = 0;
                    int maxLight = 100;
                    int lightPerLayer = (maxLight - minLight)/(blocksHigh/2);
                    float luminosity = y*lightPerLayer + minLight;
                    luminosity /= 100;
                    luminosity++;
                  red *= luminosity;
                  green *= luminosity;
                  blue *= luminosity;
                    if (red > 255)
                        red = 255;

                    if (blue > 255)
                        blue = 255;

                    if (green > 255)
                        green = 255;

                    //rectangles[x][y + i].setFillColor(sf::Color(30, 159, 0));
            //rectangles[x - blocksWide/3][blocksHigh/10 + y].setFillColor(sf::Color(red, green, blue));
        }

    //main loop
    while (true)
    {
        sf::Event action;

        while (window.pollEvent(action))
            if (action.type == sf::Event::Closed)
                return 0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            return 0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            return 0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
            goto bob;

        window.clear(sf::Color::White);

        for (int x = 0; x < blocksWide; x++)
            for (int y = 0; y < blocksHigh; y++)
                window.draw(rectangles[x][y]);

        //drawing height line for DEBUG -OMIT LATER
        //window.draw(line);
       // window.draw(line2);

        window.display();
    }
}
