/**
 * VerilogGenerator2.h
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send 
 * a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.  
 *
 */

#ifndef VERILOGGENERATOR2_H_
#define VERILOGGENERATOR2_H_


// Includes
//-----------------

//-- Std
#include <string>
using namespace std;

//-- Qt
#include <QtCore>

//-- Generator
#include <generate/Generator.h>
#include <generate/GeneratorBuilder.h>

class VerilogGenerator2: public Generator, public GeneratorBuilder {
    public:
        VerilogGenerator2();
        virtual ~VerilogGenerator2();

        /** \defgroup Builder Builder Methods */
        /** @{ */

        virtual Generator * newInstance();

        /** @} */

        /** \defgroup Generator Generator Methods */
        /** @{ */

        /**
         * Returns a short name of this Generator
         * @return
         */
        virtual QString getName();

        /**
         * Returns a short description of this Generator
         * @return
         */
        virtual QString getDescription();

        /**
         * Do the actual generation
         * @param fsm
         * @param dataStream
         */
        virtual void generate(Fsm * fsm, QDataStream * dataStream);

        /**
         * Generate an example instanciation of the created verilog module
         */
        QString createInstance(Fsm * fsm);

        /**
         * Cleans a string using regexp to make it conformant to verilog syntax
         * Ex: replaces spaces with _ etc...
         * @param input
         * @return
         */
        QString cleanString(string input);

        /** @} */
};

#endif /* VERILOGGENERATOR2_H_ */
