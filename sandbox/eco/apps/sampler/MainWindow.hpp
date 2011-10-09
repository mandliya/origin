#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <vector>
#include <fstream>

#include <boost/random/linear_congruential.hpp>

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  typedef std::vector<double> Vector;
  typedef std::vector<Vector> Data;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
  void updateOutputWindow();
  void generateData();

private slots:
  void on_populationButton_clicked();

  void on_weightsButton_clicked();

  void on_samplingChoice_currentIndexChanged(int index);

  void on_centroidalCheck_toggled(bool checked);

  void on_nearestCheck_toggled(bool checked);

  void on_hullCheck_toggled(bool checked);

  void on_generateButton_clicked();

private:
  int sampleMethod() const;
  int numSamples() const;
  bool useWeights() const;
  bool computeCentroid() const;
  bool computeNeighbor() const;
  bool computeHull() const;
  QString measureFile(QString const& measure);

  bool readPopulation(Data& data);
  bool readWeights(Vector& weights);
  void generateSamples(Data const& data, Vector const& weights);

  void computeCentroid(Data const& sample, Vector const& weights, std::ofstream& os);
  void computeNeighbor(Data const& sample, Vector const& weights, std::ofstream& os);

private:
    Ui::MainWindow *ui;

    // Output information.
    QString outputPath;
    QString outputBase;

    boost::minstd_rand randomGen;
};

#endif // MAINWINDOW_HPP
