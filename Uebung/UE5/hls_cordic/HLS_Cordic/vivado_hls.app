<project xmlns="com.autoesl.autopilot.project" name="HLS_Cordic" top="cc_cordicModule_fixed">
    <includePaths/>
    <libraryPaths/>
    <Simulation argv="">
        <SimFlow name="csim" setup="true" ldflags="-I/usr/local/systemc-2.3.2/include/ -L/usr/local/systemc-2.3.2/lib/ -lsystemc" mflags="" clean="false" csimMode="2" lastCsimMode="0" optimizeCompile="false"/>
    </Simulation>
    <files xmlns="">
        <file name="../../testbench_cc_cordicModule_fixed.cpp" sc="0" tb="1" cflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="cc_cordicModule_fixed.cpp" sc="0" tb="false" cflags="" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="HLS_Cordic" status="active"/>
    </solutions>
</project>

