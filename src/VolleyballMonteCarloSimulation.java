import java.util.ArrayList;
import java.util.Map;
import java.util.Random;

class Team {
    int index;
    String name;
    double strength;
    int wins;
    int setsWon;
    int pointsScored;
    int pointsLost;

    // 用于模拟过程中临时存储
    int tempWins;
    int tempSetsWon;
    int tempPointsScored;
    int tempPointsLost;

    Team (int index, String name, double strength) {
        this.index = index;
        this.name = name;
        this.strength = strength;
        this.wins = 0;
        this.setsWon = 0;
        this.pointsScored = 0;
        this.pointsLost = 0;
    }

    void resetTemp() {
        this.tempWins = this.wins;
        this.tempSetsWon = this.setsWon;
        this.tempPointsScored = this.pointsScored;
        this.tempPointsLost = this.pointsLost;
    }

    double getPointsRatio() {
        if (tempPointsLost == 0) return Double.MAX_VALUE;
        return (double) tempPointsScored / tempPointsLost;
    }

    @Override
    public String toString() {
        return name;
    }
}

class MatchResult {
    Team team1;
    Team team2;
    int setsWon1;
    int setsWon2;
    int points1;
    int points2;

    MatchResult(Team team1, Team team2, int setsWon1, int setsWon2, int points1, int points2) {
        this.team1 = team1;
        this.team2 = team2;
        this.setsWon1 = setsWon1;
        this.setsWon2 = setsWon2;
        this.points1 = points1;
        this.points2 = points2;
    }
}

public class VolleyballMonteCarloSimulation {
    Map<String, Team> teams;
    Map<Team, Integer> teamIndex;
    private final boolean[][] hasResult;
    private final long[] upLineCount;

    private final Random random = new Random();

    VolleyballMonteCarloSimulation(ArrayList<MatchResult> matchResults, Map<String, Team> teams, Map<Team, Integer> teamIndex) {
        this.teams = teams;
        this.teamIndex = teamIndex;
        hasResult = new boolean[teams.size()][teams.size()];
        upLineCount = new long[teams.size()];
        for (MatchResult matchResult : matchResults) {
            hasResult[matchResult.team1.index][matchResult.team2.index] = true;
            hasResult[matchResult.team2.index][matchResult.team1.index] = true;
            matchResult.team1.wins += matchResult.setsWon1 > matchResult.setsWon2 ? 1 : 0;
            matchResult.team1.setsWon += matchResult.setsWon1;
            matchResult.team1.pointsScored += matchResult.points1;
            matchResult.team1.pointsLost += matchResult.points2;
            matchResult.team2.wins += matchResult.setsWon2 > matchResult.setsWon1 ? 1 : 0;
            matchResult.team2.setsWon += matchResult.setsWon2;
            matchResult.team2.pointsScored += matchResult.points2;
            matchResult.team2.pointsLost += matchResult.points1;
        }
    }

    void run(long times) {
        for (long i = 0; i < times; i++) {
            simulateRemainingMatches();
            if ((i + 1) % 10000 == 0) {
                System.out.println();
                System.out.println("第 " + (i + 1) + " 次模拟后结果：");
                ArrayList<Team> teams_ = new ArrayList<>(teams.values());
                for (int j = 0; j < teams_.size(); j++) {
                    System.out.println(teams_.get(j) + " 出线概率：" + String.format("%.4f", (double) upLineCount[j] / (i + 1) * 100) + "%");
                }
            }
        }
    }

    // 模拟分
    private boolean simulatePoint(Team team1, Team team2) {
        double prob = team1.strength / (team1.strength + team2.strength);
        return random.nextDouble() < prob;
    }

    // 模拟局
    private int[] simulateSet(Team team1, Team team2, boolean isThirdSet) {
        int targetScore = isThirdSet ? 15 : 25;
        int score1 = 0, score2 = 0;

        while (true) {
            if (simulatePoint(team1, team2)) {
                score1++;
            } else {
                score2++;
            }

            // 检查是否达到获胜条件
            if (score1 >= targetScore && score1 - score2 >= 2) {
                break;
            }
            if (score2 >= targetScore && score2 - score1 >= 2) {
                break;
            }
        }

        return new int[]{score1, score2};
    }

    // 模拟场
    private void simulateMatch(Team team1, Team team2) {
        int setsWon1 = 0, setsWon2 = 0;

        // 模拟第一局
        int[] set1 = simulateSet(team1, team2, false);
        if (set1[0] > set1[1]) setsWon1++;
        else setsWon2++;
        team1.tempPointsScored += set1[0];
        team1.tempPointsLost += set1[1];
        team2.tempPointsScored += set1[1];
        team2.tempPointsLost += set1[0];

        // 模拟第二局
        int[] set2 = simulateSet(team1, team2, false);
        if (set2[0] > set2[1]) setsWon1++;
        else setsWon2++;
        team1.tempPointsScored += set2[0];
        team1.tempPointsLost += set2[1];
        team2.tempPointsScored += set2[1];
        team2.tempPointsLost += set2[0];

        // 模拟第三局（无论前两局结果如何）
        int[] set3 = simulateSet(team1, team2, true);
        if (set3[0] > set3[1]) setsWon1++;
        else setsWon2++;
        team1.tempPointsScored += set3[0];
        team1.tempPointsLost += set3[1];
        team2.tempPointsScored += set3[1];
        team2.tempPointsLost += set3[0];

        // 更新胜局数
        team1.tempSetsWon += setsWon1;
        team2.tempSetsWon += setsWon2;

        // 更新胜场数
        if (setsWon1 > setsWon2) team1.tempWins++;
        else team2.tempWins++;
    }

    // 模拟剩余比赛
    private void simulateRemainingMatches() {
        ArrayList<Team> teams_ = new ArrayList<>(teams.values());
        for (Team team : teams_) team.resetTemp();
        boolean[][] hasSimulated = new boolean[teams_.size()][teams_.size()];
        for (int i = 0; i < teams_.size(); i++) {
            for (int j = i + 1; j < teams_.size(); j++) {
                if (hasResult[i][j]) {
                    hasSimulated[i][j] = true;
                    hasSimulated[j][i] = true;
                }
            }
        }
        for (int i = 0; i < teams_.size(); i++) {
            for (int j = i + 1; j < teams_.size(); j++) {
                if (!hasSimulated[i][j]) {
                    simulateMatch(teams_.get(i), teams_.get(j));
                    hasSimulated[i][j] = true;
                }
            }
        }
        ArrayList<Team> sortedTeams = new ArrayList<>(teams_);
        sortedTeams.sort((t1, t2) -> {
            if (t1.tempWins != t2.tempWins) return Integer.compare(t2.tempWins, t1.tempWins);

            if (t1.tempSetsWon != t2.tempSetsWon) return Integer.compare(t2.tempSetsWon, t1.tempSetsWon);

            double ratio1 = t1.getPointsRatio();
            double ratio2 = t2.getPointsRatio();

            if (ratio1 == Double.MAX_VALUE && ratio2 == Double.MAX_VALUE) return 0;
            else if (ratio1 == Double.MAX_VALUE) return -1;
            else if (ratio2 == Double.MAX_VALUE) return 1;
            else return Double.compare(ratio2, ratio1);
        });
        for (int i = 0; i < 4; i++) {
            upLineCount[sortedTeams.get(i).index]++;
        }
    }
}