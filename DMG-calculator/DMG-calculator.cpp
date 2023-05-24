// DMG-calculator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

class diceBreakdown
{
public:
    int numberOfDice, typeOfDice, modifier;
    diceBreakdown(std::string diceString) {
        short part = 1;
        std::string dicePart[3] = { "","","" };
        for (int i = 0; i < diceString.length(); i++) {
            switch (diceString[i])
            {
            case 'D':
            case 'd':
                part = 2;
                continue;
                break;
            case '+':
            case '-':
                part = 3;
                continue;
                break;
            default:
                break;
            }
            if (diceString[i] >= '0' && diceString[i] <= '9') {
                if (part == 1)
                    dicePart[0] += diceString[i];
                else if (part == 2)
                    dicePart[1] += diceString[i];
                else if (part == 3)
                    dicePart[2] += diceString[i];
            }
        }
        int numberOfDiceT, typeOfDiceT, modifierT;
        try {
            numberOfDiceT = stoi(dicePart[0]);
        }
        catch (const std::exception&) {
            numberOfDiceT = 1;
        }
        try {
            modifierT = stoi(dicePart[2]);
        }
        catch (const std::exception&) {
            modifierT = 0;
        }
        try {
            typeOfDiceT = stoi(dicePart[1]);
        }
        catch (const std::exception&) {
            typeOfDiceT = 1;
            modifierT = numberOfDiceT;
            numberOfDiceT = 0;
        }
        this->modifier = modifierT;
        this->numberOfDice = numberOfDiceT;
        this->typeOfDice = typeOfDiceT;
    }
};
class hits {
public:
    double normal, critical;
    hits(double attacks, int crit, int toHit, std::string susHitsStr);
};

double CalculateNumberOfAttacks(std::string attacks);
double CalculateNumberOfDamage(std::string damageStr);
double CalculateNumberOfWounds(hits hitsInflicted, int lethalHits, int str, int toughness);


int main()
{
    int reset = 1;

    do {
        //Input
        int strength, AP, toHit, hitRerolls, woundRerolls, toughness, save, invurn, AOC, crit, lethalHits;
        std::string attacksStr, DMGStr, susHitsStr;
        //Output
        double finalDMG;
        std::cout << "Hello WH40K!\n";
        std::cout << "Number of attacks: ";
        std::cin >> attacksStr;
        std::cout << "Sustained hits (string): ";
        std::cin >> susHitsStr;
        std::cout << "lethal hits (1/0): ";
        std::cin >> lethalHits;
        std::cout << "Crit value (number): ";
        std::cin >> crit;
        std::cout << "Minimal dice result to hit: ";
        std::cin >> toHit;
        std::cout << "Attack strength: ";
        std::cin >> strength;
        std::cout << "AP: ";
        std::cin >> AP;
        std::cout << "Attack Damage: ";
        std::cin >> DMGStr;
        std::cout << "Target Toughness: ";
        std::cin >> toughness;
        std::cout << "Target Save Characteristic: ";
        std::cin >> save;
        std::cout << "Target Has AOC (1-YES, 0-NO): ";
        std::cin >> AOC;
        std::cout << "Target Inv Save Characteristic (0 for none): ";
        std::cin >> invurn;
        //Calculate number of attacks
        double attacksF = CalculateNumberOfAttacks(attacksStr);
        //Calculate damage:
        double DMG = CalculateNumberOfDamage(DMGStr);
        
        //Calculate hits:
        hits inflictedHits(attacksF, crit, toHit, susHitsStr);

        //Calculate wounds:
        double woundsHit = CalculateNumberOfWounds(inflictedHits, lethalHits, strength, toughness);

        std::cout << " Number of Attacks: " << attacksF << "\n";
        std::cout << " Damage of weapon: " << DMG << "\n";
        std::cout << " Hits connecting: " << inflictedHits.critical + inflictedHits.normal << "\n";
        std::cout << " Critical hits: " << inflictedHits.critical << "\n";
        std::cout << " Wounds Sticked: " << woundsHit << "\n";

        //Saves:
        double woundsPastSaves = woundsHit;
        AP -= (AOC) ? (AP > 0) ? 1 : 0 : 0;
        double saveChance = (invurn > 0 && (save + AP > invurn)) ? (7.0 - invurn) / 6.0 : (7.0 - save - AP) / 6.0;
        std::cout << " Save Chance: " << ((saveChance < 0) ? 0 : saveChance) << "\n";
        if (saveChance > 0) {
            woundsPastSaves = woundsHit - woundsHit * saveChance;
        }
        std::cout << " Wounds dealt: " << woundsPastSaves << "\n";
        //Calculate Damage:
        finalDMG = woundsPastSaves * DMG;
        std::cout << " Final Damage: " << finalDMG << "\n";
        std::cout << "Reset app (1 - yes, 0 - exit): ";
        std::cin >> reset;
    }while (reset == 1);
    return 0;
}



double CalculateNumberOfAttacks(std::string attacksStr){
    double attacksF;
    diceBreakdown attackDice(attacksStr);
    attacksF = attackDice.numberOfDice * ((double)(attackDice.typeOfDice / 2) + ((attackDice.typeOfDice % 2 == 0) ? 0.5 : 1)) + attackDice.modifier;
    return attacksF;
}

double CalculateNumberOfDamage(std::string damageStr) {
    double damageF;
    diceBreakdown damageDice(damageStr);
    damageF = damageDice.numberOfDice * ((double)(damageDice.typeOfDice / 2) + ((damageDice.typeOfDice % 2 == 0) ? 0.5 : 1)) + damageDice.modifier;
    return damageF;
}

hits::hits(double attacks, int crit, int toHit, std::string susHitsStr) {
    double nHits, cHits, hitChance, critChance;
    diceBreakdown susHits(susHitsStr);
    int localCrit = (toHit > crit) ? toHit : crit;
    hitChance = (((7.0 - (double)toHit) / 6.0));
    critChance = (((7.0 - (double)localCrit) / 6.0));
    double susHitsF = critChance * (susHits.numberOfDice * ((double)(susHits.typeOfDice / 2) + ((susHits.typeOfDice % 2 == 0) ? 0.5 : 1)) + susHits.modifier);
    nHits = attacks * (hitChance - critChance + susHitsF);
    cHits = attacks * critChance;
    critical = cHits;
    normal = nHits;
    std::cout << " ()Normal hits: " << normal << "\n";
    std::cout << " ()Critical hits: " << critical << "\n";
}

double CalculateNumberOfWounds(hits hitsInflicted, int lethalHits, int str, int toughness) {
    double woundChance, woundsInflicted;
    woundChance = (str == toughness) ? 1.0 / 2.0 : ((str > toughness) ? ((str >= toughness * 2) ? 5.0 / 6.0 : 2.0 / 3.0) : (str * 2 <= toughness) ? 1.0 / 6.0 : 1.0 / 3.0);
    woundsInflicted = woundChance * hitsInflicted.normal;
    woundsInflicted += (lethalHits == 1) ? hitsInflicted.critical : woundChance * hitsInflicted.critical;
    return woundsInflicted;
}