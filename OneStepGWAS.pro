QT       += core gui svg xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AnalyzeInfoWidget/AnalyzeInfoWidget.cpp \
    AnalyzeListView/AnalyzeListView.cpp \
    BSAVisualize/BSAIndexScatterWidget.cpp \
    BSAVisualize/BSALabelInterListView.cpp \
    BSAVisualize/BSALabelInterListWidget.cpp \
    BSAVisualize/BSAMainInfoWidget.cpp \
    BSAVisualize/BSAVisualize.cpp \
    BSAanalysis/BSAProject.cpp \
    BSAanalysis/BSAThreshold.cpp \
    FileDeal/BaseProject.cpp \
    DataInteractTable/DataInteractTableView.cpp \
    DataInteractTable/DataInteractTableWidget.cpp \
    FileDeal/GeneticModelTransFormer.cpp \
    PCAAlgo/FastPCA.cpp \
    FileDeal/BaseFileDealClass.cpp \
    FileDeal/BaseFileFormat.cpp \
    FileDeal/BaseFileSaveDialog.cpp \
    FileDeal/ProjectFilesRecord.cpp \
    FileDeal/ProjectFilesSet.cpp \
    FileDeal/ViewFileFormatWidget.cpp \
    FileDeal/FileIterator.cpp \
    GWASQQPlotVisualization/GWASQQAxisSettingWidget.cpp \
    GWASQQPlotVisualization/GWASQQColorSettings.cpp \
    GWASQQPlotVisualization/GWASQQMainInfoWidget.cpp \
    GWASQQPlotVisualization/GWASQQPlotFramePainter.cpp \
    GWASQQPlotVisualization/GWASQQPlotPointsPainter.cpp \
    GWASQQPlotVisualization/GWASQQPlotWidget.cpp \
    GWASQQPlotVisualization/GWASQQVisualizationWidget.cpp \
    GWASVisualize/ChooseGeneColorWidget.cpp \
    GWASVisualize/ChooseGeneModelColorWidget.cpp \
    GWASVisualize/GWASLabelPointListWidget.cpp \
    GWASVisualize/GWASMainInfoWidget.cpp \
    GWASVisualize/GWASSetThresholdWidget.cpp \
    GWASVisualize/GWASVisualize.cpp \
    GWASVisualize/GWASVisualizeSettingWidget.cpp \
    GWASVisualize/LogPScatterViewPainter.cpp \
    GenomeAnnotation/AxisValueCalculator.cpp \
    GenomeAnnotation/GenomeAnnoDisplayView.cpp \
    GenomeAnnotation/GenomeAnnoSlideWidget.cpp \
    GenomeAnnotation/GenomeAnnoVisualizeWidget.cpp \
    GenomeAnnotation/GenomeViewFrame.cpp \
    GenomeAnnotation/InsertTextEditorFrame.cpp \
    KinshipVisualize/KinshipHeatmapPainter.cpp \
    LoadFileWidget/LoadBSAFileWidget.cpp \
    LoadFileWidget/LoadGWASFileWidget.cpp \
    DataManager/MathSpecialFunc.cpp \
    PCAVisualize/PCA2DScatterPointsPainter.cpp \
    PCAVisualize/PCA3DScatterFramePainter.cpp \
    PCAVisualize/PCA3DScatterPointsPainter.cpp \
    PreVisualizationWidget/PreBSAVisualizeWidget.cpp \
    PreVisualizationWidget/PreKinshipVisualizeWidget.cpp \
    PreVisualizationWidget/PrePCAVisualizeWidget.cpp \
    PreVisualizationWidget/PreStrucVisualizeWidget.cpp \
    PreVisualizationWidget/PreVisualizeWidget.cpp \
    SetParamWidget/SetFastStructureParamWidget.cpp \
    SetParamWidget/SetGWASParamWidget.cpp \
    SetParamWidget/SetKinshipParamWidget.cpp \
    SetParamWidget/SetNormalBSAParamWidget.cpp \
    SetParamWidget/SetStructureParamWidget.cpp \
    SpecialWidget/LoadingWidget.cpp \
    StartVisualization/GeneFindList.cpp \
    GWASVisualize/GeneInfoWidget.cpp \
    GWASVisualize/InfoDisplayWidget.cpp \
    GWASVisualize/LabelPointsListView.cpp \
    GWASVisualize/LogPScatterWidget.cpp \
    GWASVisualize/PointInfoWidget.cpp \
    GenomeAnnotation/GenomeAnnotation.cpp \
    KinshipCalcuAlgo/KinshipCalculator.cpp \
    KinshipVisualize/KinshipHeatmapWidget.cpp \
    KinshipVisualize/KinshipVisualizeWidget.cpp \
    PCAVisualize/PCA2DScatterPlotWidget.cpp \
    PCAVisualize/PCA2DVisualizationWidget.cpp \
    PCAVisualize/PCA3DScatterPlotWidget.cpp \
    PCAVisualize/PCA3DVisualizationWidget.cpp \
    PCAVisualize/PCAMainInfoWidget.cpp \
    ProjectTreeView/ProjectTreeView.cpp \
    StyleChoose/SetPointSizeWidget.cpp \
    ThreadProcess/QThreadRun.cpp \
    GFFSetting/SetGffReadAttribute.cpp \
    StartVisualization/ChooseColorListWidget.cpp \
    StartVisualization/IndividualSearchWidget.cpp \
    StartVisualization/LoadGenomeSequenceWidget.cpp \
    StartVisualization/StartGWASQQVisualizationWidget.cpp \
    StartVisualization/StartKinVisualizationWidget.cpp \
    StartVisualization/StartStrucVisualizationWidget.cpp \
    StartVisualization/StartGWASVisualizationWidget.cpp \
    StartVisualization/StartPCAVisualizationWidget.cpp \
    StartVisualization/StringFindListItemDelegate.cpp \
    StructureAlgo/Structure.cpp \
    StructureVisualize/StrucVisualSettingsWidget.cpp \
    StructureVisualize/StructureBarPainter.cpp \
    StructureVisualize/StructureBarPlotWidget.cpp \
    StructureVisualize/StructureMainInfoWidget.cpp \
    StructureVisualize/StructureVisualizeWidget.cpp \
    StyleChoose/LineStyleChoose.cpp \
    ThreadProcess/ThreadRun.cpp \
    DataManager/ValPoints.cpp \
    Debug/debug.cpp \
    StructureAlgo/FastStructure.cpp \
    WidgetUtil/WidgetUtil.cpp \
    main.cpp \
    MainWindow/mainwindow.cpp \
    DataManager/DataManager.cpp \
    EMMAXAlgo/EMMAX.cpp \
    GEMMAAlgo/GEMMA.cpp \
    FileDeal/FileDeal.cpp \
    DataManager/MatrixLib.cpp

HEADERS += \
    AnalyzeInfoWidget/AnalyzeInfoWidget.h \
    AnalyzeListView/AnalyzeListView.h \
    BSAVisualize/BSAIndexScatterWidget.hpp \
    BSAVisualize/BSALabelInterListView.hpp \
    BSAVisualize/BSALabelInterListWidget.hpp \
    BSAVisualize/BSAMainInfoWidget.hpp \
    BSAVisualize/BSAVisualize.hpp \
    BSAanalysis/BSAProject.hpp \
    BSAanalysis/BSAThreshold.hpp \
    FileDeal/BaseProject.hpp \
    Cluster/Cluster.hpp \
    DataManager/DataDefine.hpp \
    DataInteractTable/DataInteractTableView.hpp \
    DataInteractTable/DataInteractTableWidget.hpp \
    FileDeal/GeneticModelTransFormer.h \
    PCAAlgo/FastPCA.hpp \
    FileDeal/BaseFileDealClass.hpp \
    FileDeal/BaseFileFormat.hpp \
    FileDeal/BaseFileSaveDialog.hpp \
    FileDeal/FileFormClass.hpp \
    FileDeal/ProjectFilesRecord.hpp \
    FileDeal/ProjectFilesSet.h \
    FileDeal/ViewFileFormatWidget.hpp \
    FileDeal/FileIterator.hpp \
    GWASQQPlotVisualization/GWASQQAxisSettingWidget.h \
    GWASQQPlotVisualization/GWASQQColorSettings.h \
    GWASQQPlotVisualization/GWASQQMainInfoWidget.h \
    GWASQQPlotVisualization/GWASQQPlotFramePainter.h \
    GWASQQPlotVisualization/GWASQQPlotPointsPainter.h \
    GWASQQPlotVisualization/GWASQQPlotWidget.h \
    GWASQQPlotVisualization/GWASQQVisualizationWidget.h \
    GWASVisualize/ChooseGeneColorWidget.hpp \
    GWASVisualize/ChooseGeneModelColorWidget.hpp \
    GWASVisualize/GWASLabelPointListWidget.hpp \
    GWASVisualize/GWASMainInfoWidget.hpp \
    GWASVisualize/GWASSetThresholdWidget.hpp \
    GWASVisualize/GWASThresholdPainter.hpp \
    GWASVisualize/GWASVisualize.hpp \
    GWASVisualize/GWASVisualizeSettingWidget.hpp \
    GWASVisualize/LogPScatterViewPainter.hpp \
    GenomeAnnotation/AxisValueCalculator.h \
    GenomeAnnotation/BaseOpenGLPaintWidget.hpp \
    GenomeAnnotation/ChromosomeInfoClass.hpp \
    GenomeAnnotation/GenomeAnnoDisplayView.hpp \
    GenomeAnnotation/GenomeAnnoSlidePainter.hpp \
    GenomeAnnotation/GenomeAnnoSlideWidget.h \
    GenomeAnnotation/GenomeAnnoVisualizeWidget.hpp \
    GenomeAnnotation/GenomeViewFrame.hpp \
    GenomeAnnotation/GenomeViewTextEditPainter.hpp \
    GenomeAnnotation/GenomeViewTextPainter.hpp \
    GenomeAnnotation/InsertTextEditorFrame.hpp \
    GenomeAnnotation/WidgetTextPainter.hpp \
    KinshipVisualize/KinshipFramePainter.hpp \
    KinshipVisualize/KinshipHeatmapPainter.hpp \
    KinshipVisualize/KinshipLegendPainter.hpp \
    KinshipVisualize/KinshipRangeValColSettingWidget.hpp \
    LoadFileWidget/LoadBSAFileWidget.hpp \
    LoadFileWidget/LoadFileWidgetHeader.hpp \
    LoadFileWidget/LoadGWASFileWidget.hpp \
    DataManager/MathSpecialFunc.hpp \
    PCAVisualize/PCA2DScatterFramePainter.hpp \
    PCAVisualize/PCA2DScatterPointsPainter.hpp \
    PCAVisualize/PCA3DScatterFramePainter.hpp \
    PCAVisualize/PCA3DScatterPointsPainter.hpp \
    PCAVisualize/PCAScatterLegendPainter.hpp \
    PreVisualizationWidget/PreBSAVisualizeWidget.hpp \
    PreVisualizationWidget/PreKinshipVisualizeWidget.hpp \
    PreVisualizationWidget/PrePCAVisualizeWidget.hpp \
    PreVisualizationWidget/PreStrucVisualizeWidget.hpp \
    PreVisualizationWidget/PreVisualizationWidgetHeader.hpp \
    PreVisualizationWidget/PreVisualizeWidget.hpp \
    SetParamWidget/SetFastStructureParamWidget.hpp \
    SetParamWidget/SetGWASParamWidget.hpp \
    SetParamWidget/SetKinshipParamWidget.hpp \
    SetParamWidget/SetNormalBSAParamWidget.hpp \
    SetParamWidget/SetParamWidgetHeader.hpp \
    SetParamWidget/SetStructureParamWidget.hpp \
    ShapePainter/FramePainter.hpp \
    SpecialWidget/LoadingWidget.hpp \
    StartVisualization/GeneFindList.hpp \
    GWASVisualize/GeneInfoWidget.hpp \
    GWASVisualize/InfoDisplayWidget.hpp \
    GWASVisualize/LabelPointsListView.hpp \
    GWASVisualize/LogPScatterWidget.h \
    GWASVisualize/PointInfoWidget.hpp \
    GenomeAnnotation/GenomeAnnotation.hpp \
    KinshipCalcuAlgo/KinshipCalculator.hpp \
    KinshipVisualize/KinshipHeatmapWidget.hpp \
    KinshipVisualize/KinshipVisualizeWidget.hpp \
    PCAVisualize/PCA2DScatterPlotWidget.hpp \
    PCAVisualize/PCA2DVisualizationWidget.hpp \
    PCAVisualize/PCA3DScatterPlotWidget.hpp \
    PCAVisualize/PCA3DVisualizationWidget.hpp \
    PCAVisualize/PCAMainInfoWidget.hpp \
    ProjectTreeView/ProjectTreeView.hpp \
    StyleChoose/SetPointSizeWidget.h \
    ThreadProcess/QThreadRun.hpp \
    GFFSetting/SetGffReadAttribute.hpp \
    StartVisualization/ChooseColorListWidget.hpp \
    StartVisualization/IndividualSearchWidget.hpp \
    StartVisualization/LoadGenomeSequenceWidget.hpp \
    StartVisualization/StartGWASQQVisualizationWidget.h \
    StartVisualization/StartKinVisualizationWidget.hpp \
    StartVisualization/StartStrucVisualizationWidget.hpp \
    StartVisualization/StartGWASVisualizationWidget.hpp \
    StartVisualization/StartPCAVisualizationWidget.hpp \
    StartVisualization/StringFindListItemDelegate.hpp \
    StructureAlgo/Structure.hpp \
    StructureVisualize/SizeSettingWidget.hpp \
    StructureVisualize/StrucVisualSettingsWidget.hpp \
    StructureVisualize/StructureBarPainter.hpp \
    StructureVisualize/StructureBarPlotWidget.hpp \
    StructureVisualize/StructureFramePainter.hpp \
    StructureVisualize/StructureMainInfoWidget.hpp \
    StructureVisualize/StructureSortPaintSeq.hpp \
    StructureVisualize/StructureVisualizeWidget.hpp \
    StyleChoose/ColorChoosePushButton.hpp \
    StyleChoose/LineStyleChoose.hpp \
    StyleChoose/LineStyleParam.hpp \
    StyleChoose/RectSizeDisplayWidget.hpp \
    ThreadProcess/ThreadRun.h \
    DataManager/ValPoints.hpp \
    Debug/debug.hpp \
    StructureAlgo/FastStructure.hpp \
    MainWindow/mainwindow.h \
    DataManager/DataManager.hpp \
    EMMAXAlgo/EMMAX.hpp \
    GEMMAAlgo/GEMMA.hpp \
    DataManager/MatrixLib.hpp \
    FileDeal/FileDeal.hpp \
    WidgetUtil/WidgetUtil.hpp \
    openblas/*

FORMS += \
    BSAVisualize/BSAMainInfoWidget.ui \
    BSAVisualize/BSAVisualize.ui \
    DataInteractTable/DataInteractTableWidget.ui \
    GWASQQPlotVisualization/GWASQQMainInfoWidget.ui \
    GWASQQPlotVisualization/GWASQQVisualizationWidget.ui \
    GWASVisualize/ChooseGeneColorWidget.ui \
    GWASVisualize/ChooseGeneModelColorWidget.ui \
    GWASVisualize/GWASLabelPointListWidget.ui \
    GWASVisualize/GWASMainInfoWidget.ui \
    GWASVisualize/GWASVisualize.ui \
    GenomeAnnotation/InsertTextEditorFrame.ui \
    StartVisualization/GeneFindList.ui \
    GWASVisualize/GeneInfoWidget.ui \
    GWASVisualize/InfoDisplayWidget.ui \
    GWASVisualize/PointInfoWidget.ui \
    KinshipVisualize/KinshipVisualizeWidget.ui \
    PCAVisualize/PCA2DVisualizationWidget.ui \
    PCAVisualize/PCA3DVisualizationWidget.ui \
    PCAVisualize/PCAMainInfoWidget.ui \
    GFFSetting/SetGffReadAttribute.ui \
    StructureVisualize/StrucVisualSettingsWidget.ui \
    StructureVisualize/StructureMainInfoWidget.ui \
    StructureVisualize/StructureVisualizeWidget.ui \
    MainWindow/mainwindow.ui

LIBS += libopenblas.dll

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    Res.qrc

#win32{
#    QMAKE_POST_LINK += xcopy
#}
