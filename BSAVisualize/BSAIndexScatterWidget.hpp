#ifndef BSAINDEXSCATTERWIDGET_HPP
#define BSAINDEXSCATTERWIDGET_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include "FileDeal/FileDeal.hpp"
#include "BSAanalysis/BSAProject.hpp"
#include "GenomeAnnotation/GenomeAnnotation.hpp"

class BSAPointID{
public:
    size_t id;
    short info;
    //-1:NULL 0:HPIndex 1:LPIndex 2:DeltaIndex 3:upThreshold 4:downThreshold
    explicit BSAPointID();
    BSAPointID(size_t inID,short inInfo);
    bool operator==(const BSAPointID& B) const;
};


class BSAIndexScatterWidget : public QOpenGLWidget,public QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
private:
    explicit BSAIndexScatterWidget(QWidget *parent = nullptr);
public:
    int widgetLeftMargin;
    int widgetTopMargin;
    float scatterAreaHeightRatio; //0~1
        // scatterAreaHeightRatio=scatterAreaHeight/(width()-widgetTopMargin)
    float subScatterAreaHeightRatio; //0~0.5
        // subScatterAreaHeight = subScatterAreaHeightRatio*scatterAreaHeight
        //                      = subScatterAreaHeightRatio*scatterAreaHeightRatio*(width()-widgetTopMargin)
    int scatterScaleLen;
    int zoomFactor;
    size_t minPosGapLen;
    int geneModelHeight;
    int baseHeight;
    int positionLabelHeight;
    int positionLabelWidth;
    float moveFactor;
    size_t geneDisplayMaxPosGapLen;
    int maxLabelNum;
    float selectedPointsDistance2;
    short geneDisplayMode;
    int maxLabelPointsNum;
    bool isDisplayMajorGridline;
    bool isDisplayMinorGridline;
    bool isDisplayGeneModel;
    bool isDisplayGenomeSeq;
    QFont geneNameFont;
    QVector3D iResolution;

    double indexChartMaximun;
    double indexChartMinimun;
    double deltaChartMaximun;
    double deltaChartMinimun;

    QColor colorScatterPoint;
    QColor colorSelectedFrame;
    QColor colorForwardGene;
    QColor colorReverseGene;
    QColor colorNullStrandGene;
    QColor colorDefaultGene;
    QColor color5UTR;
    QColor color3UTR;
    QColor colorCDS;
    QColor baseA;
    QColor baseT;
    QColor baseC;
    QColor baseG;
    QColor baseN;
    QColor base_;

    QColor colorHP;
    QColor colorLP;
    QColor colorDP;
    QColor colorTP;

    long long *chrLen;
    long long *cumChrLen;
    char **chrName;
    int nChr;
    long long maxCumPos;

    FD::BSAInterVal* points;
    long long nPoints;
    long long *cumLPointsPos;
    long long *cumRPointsPos;

    ValPoints2* markers;
    long long nMarkers;
    long long *cumMarkersPos;

    GLfloat *paintHPoints;
    GLfloat *paintLPoints;
    GLfloat *paintDPoints;
    GLfloat *paintT1Points;
    GLfloat *paintT2Points;
    long long nPaintPoints;

    GLfloat *paintHMarkers;
    GLfloat *paintLMarkers;
    GLfloat *paintDMarkers;
    long long nPaintMarkers;

    long double currentLeftPosF;
    long double currentRightPosF;
//    long long currentLeftPos;
//    long long currentRightPos;
    long long currentMinPointsi;
    long long currentMaxPointsi;
    long long currentMinMarkersi;
    long long currentMaxMarkersi;

    int currentMinChri;
    int currentMaxChri;

    long long currentMinGenei;
    long long currentMaxGenei;
    long long nPaintGenes;

    bool isLeftMousePressed;
    bool isMiddleMousePressed;
    bool isRightMousePressed;
    int leftMousePressedPosX;
    int leftMouseReleasedPosX;
    int leftMouseMovingPosX;
    int middleMousePressedPosX;
    int middleMouseMovingPosX;
    int RightMousePressedPosX;
    int RightMousePressedPosY;
    long double middleMousePressedLeftPosF;
    long double middleMousePressedRightPosF;

    BSAPointID selectedPointi;
    long long selectedGenei;

    GenomeAnnotation* genome;
    GenomeSequence*   genomeSeq;

    QMenu* pMenu;
    QAction* actAddLabelPoint;
    QAction* actremoveLabelPoint;

    QVector<BSAPointID> idLabelPoints;
    BSAPointID idRightPressPoint;

    QOpenGLShaderProgram shaderProgram;
    GLuint idPointsVBOH;
    GLuint idPointsVBOL;
    GLuint idPointsVBOD;
    GLuint idPointsVBOT1;
    GLuint idPointsVBOT2;
    GLuint idPointsVAOH;
    GLuint idPointsVAOL;
    GLuint idPointsVAOD;
    GLuint idPointsVAOT1;
    GLuint idPointsVAOT2;
    GLuint idMarkersVAOH;
    GLuint idMarkersVAOL;
    GLuint idMarkersVAOD;
    GLuint idMarkersVBOH;
    GLuint idMarkersVBOL;
    GLuint idMarkersVBOD;

    BSAIndexScatterWidget(size_t *inChrLen,char** inChrName,int inNChr,
                          FD::BSAInterVal* inPoints,size_t inNPoints,
                          ValPoints2* inMarkers,size_t inNMarkers,
                          QWidget *parent = nullptr);
    ~BSAIndexScatterWidget();
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;

    bool isSelectedPoint(long long pointi,int x,int y,short info); // 0:HPIndex 1:LPIndex 2:DeltaIndex
                                                                   // 3:upThreshold 4:downThreshold
    bool isSelectedGene(long long genei,int x,int y);

    bool loadPoints(FD::BSAInterVal* inPoints,long long inNPoints);
    bool loadChromosome(size_t *inChrLen,char** inChrName,int inNChr);

    bool loadGenomeSeq(const char* inFile);
    bool loadMarkers(ValPoints2* inMarkers,long long inNMarkers);
    bool createGenome();
    bool createGenome(char** inChrName,int inNChr,long long inNGenes);
    bool setGenome(const GenomeAnnotation& inGenome);
    bool mouseXToPointX(int x1,int x2);
    BSAPointID isPointPos(int x,int y);
    double calcuChartX(long long idChr,long double pos);
    double calcuChartY(double val,short info);
    double calcuChartY(const BSAPointID& p);
    void dealLeftMousePressPoint(int x,int y);
    void dealLeftMousePressGene(int x,int y);
    void dealAddPointLabel();
    void dealRemovePointLabel();

    void paintAxisInPainter(QPainter *painter);
    void paintVerticalLineInPainter(QPainter *painter);
    void paintZoomRegionLineInPainter(QPainter *painter);
    void paintGenesInPainter(QPainter *painter);
    void paintGenesModelInPainter(QPainter *painter);
    void paintSelectedPoint(QPainter *painter);
    void paintSelectedGene(QPainter *painter);
    void paintGenomeSeq(QPainter *painter);
    void paintOneBase(char inBase,
                      int xPos,
                      int oneBaseLen,
                      QPainter *painter);
    void paintLabelPoints(QPainter *painter);

    void paintPointsToBuffer();
    void paintMarkersToBuffer();
    void adjustPaintPointsPos();
    void adjustPaintMarkersPos();
    void adjustPaintChri();
    void adjustPaintGenesi();

    void lookGene(long long iGene);
    void lookInterval(int leftIdChr,int rightIdChr,
                      long double leftChrPos,long double rightChrPos);
    void getCurrentInterval(int& leftIdChr,long double& leftChrPos,
                              int& rightIdChr,long double& rightChrPos);
    GenomeAnnotation* getGenome();
    FD::BSAInterVal* getPoints();
    size_t getNPoints();
    char** getChrName();
    QColor getPointsColor() const;
    QColor getForwardGeneColor() const;
    QColor getReverseGeneColor() const;
    QColor getDefaultGeneColor() const;
    QColor get5UTRColor() const;
    QColor get3UTRColor() const;
    QColor getCDSColor() const;
    int    getMaxLabelPointsNum() const;

    void zoomIn();
    void zoomOut();

    void moveLeft();
    void moveRight();

    void makeMenu();

//    void paintCurrent(QPainter *painter); //pass

    void setDisplayMajorGridline(bool flag);
    void setDisplayMinorGridline(bool flag);
    void switchGeneDisplayMode();
    void setDisplayGene(bool flag);
    void setDisplayGeneModel(bool flag);
    void setDisplayGenome(bool flag);
    void setForwardGeneColor(QColor inColor,bool isUpdate = true);
    void setReverseGeneColor(QColor inColor,bool isUpdate = true);
    void setDefaultGeneColor(QColor inColor,bool isUpdate = true);
    void set5UTRColor(QColor inColor,bool isUpdate = true);
    void set3UTRColor(QColor inColor,bool isUpdate = true);
    void setCDSColor(QColor inColor,bool isUpdate = true);

    void doRemoveAllLabelPoint();
    void doRemoveLabelPoint(int row);
    void doLookLabelPoint(int row);

    static int chrNameMaxLen;
signals:
    void pointSelected(BSAPointID ip);
    void geneSelected(size_t id);
    void removeLabelPoint(int row);
    void addLabelPoint(BSAPointID ip);
    void currentIntervalChanged();
protected:
    void init();
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
public slots:
    void menuExec(const QPoint& pos);
};

#endif // BSAINDEXSCATTERWIDGET_HPP
